#!/bin/bash

for sem in all_shortest_trails all_shortest_simple all
do
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_diamond_mdb.py $sem bfs naive btree
done

for sem in simple trails any_simple any_trails any
do
    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_diamond_mdb.py $sem bfs naive btree

    sync; echo 3 > /proc/sys/vm/drop_caches
    python3 scripts/benchmark_diamond_mdb.py $sem dfs naive btree
done
