#!/bin/bash

############# EDIT THIS: #############
RPQ_PAPER_HOME=/home/user/RPQPaper
######################################

TRIE_STATS=$RPQ_PAPER_HOME/MillenniumDB/trie_stats.csv
RESULTS_PATH=$RPQ_PAPER_HOME/results
QUERIES=$RPQ_PAPER_HOME/queries/wdbench_paths.txt
QUERIES_DUPLICATED=$RPQ_PAPER_HOME/queries/wdbench_paths_cache.txt

for sem in all_shortest_trails all_shortest_simple all
do
    # trie naive
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC bfs naive trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-bfs_naive_trie.csv $TRIE_STATS > $RESULTS_PATH/stat_$SEMANTIC-bfs_naive_trie.csv

    # trie cache
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC bfs cache trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-bfs_cache_trie.csv $TRIE_STATS > $RESULTS_PATH/stat_$SEMANTIC-bfs_cache_trie.csv

    # trie full cache
    sync; echo 3 > /proc/sys/vm/drop_caches
    rm $RESULTS_PATH/$SEMANTIC-bfs_cache_trie.csv
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES_DUPLICATED $SEMANTIC bfs cache trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-bfs_cache_trie.csv $TRIE_STATS > $RESULTS_PATH/tmp_stat_$SEMANTIC-bfs_fullcache_trie.csv
    tail -n 592 $RESULTS_PATH/tmp_stat_$SEMANTIC-bfs_fullcache_trie.csv > $RESULTS_PATH/stat_$SEMANTIC-bfs_fullcache_trie.csv
    rm $RESULTS_PATH/tmp_stat_$SEMANTIC-bfs_fullcache_trie.csv

    # btree
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC bfs naive btree
done

for SEMANTIC in simple trails any_simple any_trails any
do
    # trie naive
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC bfs naive trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-bfs_naive_trie.csv $TRIE_STATS > $RESULTS_PATH/stat_$SEMANTIC-bfs_naive_trie.csv

    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC dfs naive trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-dfs_naive_trie.csv $TRIE_STATS > $RESULTS_PATH/stat_$SEMANTIC-dfs_naive_trie.csv

    # trie cache
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC bfs cache trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-bfs_cache_trie.csv $TRIE_STATS > $RESULTS_PATH/stat_$SEMANTIC-bfs_cache_trie.csv

    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC dfs cache trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-dfs_cache_trie.csv $TRIE_STATS > $RESULTS_PATH/stat_$SEMANTIC-dfs_cache_trie.csv

    # trie full cache
    sync; echo 3 > /proc/sys/vm/drop_caches
    rm $RESULTS_PATH/$SEMANTIC-bfs_cache_trie.csv
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES_DUPLICATED $SEMANTIC bfs cache trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-bfs_cache_trie.csv $TRIE_STATS > $RESULTS_PATH/tmp_stat_$SEMANTIC-bfs_fullcache_trie.csv
    tail -n 592 $RESULTS_PATH/tmp_stat_$SEMANTIC-bfs_fullcache_trie.csv > $RESULTS_PATH/stat_$SEMANTIC-bfs_fullcache_trie.csv
    rm $RESULTS_PATH/tmp_stat_$SEMANTIC-bfs_fullcache_trie.csv

    sync; echo 3 > /proc/sys/vm/drop_caches
    rm $RESULTS_PATH/$SEMANTIC-dfs_cache_trie.csv
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES_DUPLICATED $SEMANTIC dfs cache trie
    python3 scripts/process_trie_stats.py $RESULTS_PATH/$SEMANTIC-dfs_cache_trie.csv $TRIE_STATS > $RESULTS_PATH/tmp_stat_$SEMANTIC-dfs_fullcache_trie.csv
    tail -n 592 $RESULTS_PATH/tmp_stat_$SEMANTIC-dfs_fullcache_trie.csv > $RESULTS_PATH/stat_$SEMANTIC-dfs_fullcache_trie.csv
    rm $RESULTS_PATH/tmp_stat_$SEMANTIC-dfs_fullcache_trie.csv

    # btree
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC bfs naive btree

    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_wdbench_paths_mdb.py $QUERIES $SEMANTIC dfs naive btree
done
