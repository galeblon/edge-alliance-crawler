from math import sqrt
from urllib.request import Request, urlopen, URLError
from urllib.parse import urljoin, urlparse
from bs4 import BeautifulSoup
from igraph import *
import ssl
from django.utils.encoding import iri_to_uri
import matplotlib.pyplot as plt
import time
import socket
import networkx as nx
import json
import random
import threading
import multiprocessing

CREATE_NODE = 0
CREATE_EDGE = 1

urlblacklist = set([
    "www.youtube.com",
    "www.facebook.com",
    "www.google.com"
])

ThreadFinished = 0

# polecam stronkę https://www.york.ac.uk/teaching/cws/wws/webpage1.html - stosunkowo mało linków. Depth 5 to już długie czekanie, do 4 jest ok.

class Crawler(threading.Thread):
    def __init__(self, id, max_depth, links_to_crawl, marked_pages, domainDict, graphQueue):

        threading.Thread.__init__(self)
        print(f"Web Crawler worker {id} has Started")


        self.id = id
        self.domainDict = domainDict
        self.graphQueue = graphQueue

        self.links_to_crawl = links_to_crawl

        # słownik stron, które są będą lub na pewno nie będą odwiedzone + parametr depth
        self.marked_pages = marked_pages

        self.max_depth = max_depth

        self.my_ssl = ssl.create_default_context()

        # wyłączona weryfikacja certyfikatów
        self.my_ssl.check_hostname = False
        self.my_ssl.verify_mode = ssl.CERT_NONE

    def run(self):
        global ThreadFinished
        while True:
            try:
                link = self.links_to_crawl.get(timeout=20)
            except:
                print(f"[{self.id}] Thread finished")
                ThreadFinished = ThreadFinished + 1
                return
            try:
                domain = urlparse(link)[1]

                if domain in self.domainDict.keys():
                    rndm = random.uniform(0, 15)
                    sq = sqrt(self.domainDict[domain])
                    if rndm < sq:
                        continue

                print(f"[{self.id}] Currently processing {link}")
                req = Request(link, headers= {'User-Agent': 'Mozilla/5.0'})
                response = urlopen(req, context = self.my_ssl, timeout=1)

                soup = BeautifulSoup(response.read(),"html.parser")

                i = 0
                all_tags = soup.find_all('a')
                while i < len(all_tags) and i < 20 + sqrt(len(all_tags)):
                    child_link = all_tags[i].get("href")
                    i = i + 1

                    child_link = urljoin(link, child_link)
                    child_link = iri_to_uri(child_link)

                    # sprawdz blackliste
                    child_domain = urlparse(child_link)[1]
                    if (child_domain in urlblacklist) or child_link.endswith('.pdf'):
                        continue

                    if child_domain in self.domainDict.keys():
                        # losuj zignorowanie na podstawie ilości danej domeny
                        rndm = random.uniform(0, 15)
                        sq = sqrt(self.domainDict[child_domain])
                        if rndm < sq:
                            continue 
                    else:
                        self.domainDict[child_domain] = 0

                    child_link_marked = child_link in self.marked_pages

                    if not child_link_marked:
                        self.graphQueue.put([CREATE_NODE, child_link])
                        child_depth = self.marked_pages[link] + 1
                        self.marked_pages[child_link] = child_depth

                    # dodaj krawędź tylko jeżeli jej jeszcze nie ma
                    self.graphQueue.put([CREATE_EDGE, link, child_link])
                    #if not child_link in self.graph.neighbors(link):
                        #self.graph.add_edge(link, child_link)

                    # dodaj do kolejki
                    if (not child_link_marked) and (child_depth <= self.max_depth):
                        self.links_to_crawl.put(child_link)
                        self.domainDict[child_domain] = self.domainDict[child_domain] + 1

            except URLError as e:
                print(f"ERROR - {e.reason}")
            except socket.timeout as e:
                print('Socket timeout')
            except:
                print('Other error')
            
            print(f"[{self.id}] Queue Size: {self.links_to_crawl.qsize()}")

if __name__ == '__main__':

    print("The crawler is started\n")

    SHOW_GRAPH = False
    DRAW_NODE_LABELS = False

    base_url = input("Please enter website to crawl > ")
    max_depth = int(input("Please enter max depth > "))
    totalCrawlers = int(input("Please enter number of crawlers > "))

    start = time.time()

    graph = nx.Graph()

    links_to_crawl = multiprocessing.Queue()

    graphQueue = multiprocessing.Queue()

    graphQueue.put([CREATE_NODE, base_url])

    # znaki nie-ascii w url
    base_url = iri_to_uri(base_url)
    links_to_crawl.put(base_url)

    marked_pages = {}
    marked_pages[base_url] = 0

    domainDict = {}

    for i in range(totalCrawlers):
        crawler = Crawler(id = i,
                          max_depth = max_depth,
                          links_to_crawl = links_to_crawl,
                          marked_pages = marked_pages,
                          domainDict = domainDict,
                          graphQueue = graphQueue)
        crawler.start()

    nodes = 0
    edges = 0

    while True:
        try:
            queueItem = graphQueue.get(timeout=20)
            if queueItem[0] == CREATE_NODE:
                graph.add_node(queueItem[1])
                nodes = nodes + 1
            elif not (queueItem[2] in graph.neighbors(queueItem[1])):
                graph.add_edge(queueItem[1], queueItem[2])
                edges = edges + 1
        except:
            if ThreadFinished == totalCrawlers:
                print('\nFinished crawling after %s min.\nTotal nodes: %d\nTotal edges: %d\n' % (((time.time() - start) / 60), nodes, edges))
                break

    if SHOW_GRAPH:
        print('Plotting collected data...')
        pos = nx.spring_layout(graph)
        nx.draw_networkx_nodes(graph, pos, cmap=plt.get_cmap('jet'), node_size=100)
        if DRAW_NODE_LABELS:
            nx.draw_networkx_labels(graph, pos, font_size=8)
        nx.draw_networkx_edges(graph, pos, arrows=False)
        plt.show()

    start = time.time()
    print('Parsing collected data into graph6 format...')
    nx.write_graph6(graph, "graph.g6", header=False)
    print("Parsing finished. Output file - graph.g6")
    print("Preparing json file...")
    g6 = open("graph.g6", "r").readline()[:-1] # skip newline
    nodeDesctiptions = []
    for node in graph.nodes():
        nodeDesctiptions.append({'address': node})
    
    graphDescription = {'graph': g6, 'meta': nodeDesctiptions}
    js = json.dumps(graphDescription, indent=4)

    final = open('output.json', "w").write(js)
    print(f"Json ready after {(time.time() - start) / 60} min. Output file - output.json")
