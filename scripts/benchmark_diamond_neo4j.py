from neo4j import GraphDatabase
import time

def execute_query(session, query, results_file, i):
    result_count = 0
    start_time = time.time()
    try:
        results = session.execute_read(lambda tx: tx.run(query).data())
        for _ in results:
            result_count += 1
        elapsed_time = int((time.time() - start_time) * 1000)
        results_file.write(f'\'{i}\t{result_count}\tOK\t{elapsed_time}\n')

    except Exception as e:
        elapsed_time = int((time.time() - start_time) * 1000)
        results_file.write(f'\'{i}\t{result_count}\tERROR({type(e).__name__})\t{elapsed_time}\n')
        print(e)

    results_file.flush()


driver = GraphDatabase.driver('bolt://127.0.0.1:7687')
with driver.session(database='diamond1000') as session:
    with open('NEO4J_DIAMOND_ALL_TRAILS.tsv', 'w') as results_file:
        results_file.write('query\tresults\tstatus\ttime\n')
        for i in [10,10,20,30,40,50,60,70,80,90,100]:
            query = f'MATCH p=( ({{id:"N{3000-(3*i)}"}})-[:A*0..]->({{id:"N3000"}}) ) RETURN p LIMIT 100000;'
            print(query)
            execute_query(session, query, results_file, i)

    with open('NEO4J_DIAMOND_SHORTEST_PATH.tsv', 'w') as results_file:
        results_file.write('query\tresults\tstatus\ttime\n')
        for i in [10,10,20,30,40,50,60,70,80,90,100]:
            query = f'MATCH p=shortestPath( ({{id:"N{3000-(3*i)}"}})-[:A*0..]->({{id:"N3000"}}) ) RETURN p LIMIT 100000;'
            print(query)
            execute_query(session, query, results_file, i)

    with open('NEO4J_DIAMOND_ALL_SHORTEST_PATHS.tsv', 'w') as results_file:
        TIMEOUTS = 0
        results_file.write('query\tresults\tstatus\ttime\n')
        for i in [10,10,20,30,40,50,60,70,80,90,100]:
            query = f'MATCH p=allShortestPaths( ({{id:"N{3000-(3*i)}"}})-[:A*0..]->({{id:"N3000"}}) ) RETURN p LIMIT 100000;'
            print(query)
            execute_query(session, query, results_file, i)
driver.close()
