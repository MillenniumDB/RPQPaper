This repository has the source code and the scripts user for the experiments in the paper:

**Evaluating Regular Path Queries in GQL and SQL/PGQ: How Far Can The Classical Algorithms Take Us?**

TODO: link to extended version:

# Replicating the experiments

- Clone this repo
- Enter to this project root directory
    - `cd RPQPaper`

- Set RPQ_PAPER_HOME with the path of this project root directory
    - `export RPQ_PAPER_HOME=$(pwd)`

## MillenniumDB
These instructions works on Ubuntu 20.04. Other linux distributions might need to install dependencies differently. Libboost version used is 1.71.0, with other versions MillenniumDB might not compile.

- Install MillenniumDB dependencies.
    - `sudo apt update`
    - `sudo apt install g++ cmake libboost-all-dev`

- Change directory to the MillenniumDB folder
    - `cd MillenniumDB`

- Compile MillenniumDB:
    - `cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release && cmake --build build/Release/`

- Create **WDBench** database:
    - Download WDBench data from [Figshare](https://figshare.com/s/50b7544ad6b1f51de060) and extract it in the `data` folder.
    - `cd $RPQ_PAPER_HOME`
    - `python3 scripts/wdbench_nt_to_neo4j.py data/truthy_direct_properties.nt data/wdbench.mdb`
    - `cd MillenniumDB`
    - `build/Release/bin/create_db ../data/wdbench.mdb dbs/wdbench`

- Create the **diamond1000** database:
    - `build/Release/bin/create_db ../data/diamond1000.mdb dbs/diamond1000`

- Go back to this project root directory
    - `cd ..`

- Run WDBench paths benchmark:
    - Edit the param `RPQ_PAPER_HOME` in `scripts/benchmark_wdbench_paths_mdb.sh` and `scripts/benchmark_wdbench_paths_mdb.py`
    - The script needs to be executed as root user to clear the cache of the operating system:
        - `sudo su -`
    - change directory to `$RPQ_PAPER_HOME`
    - `bash scripts/benchmark_wdbench_paths_mdb.sh`

- Run diamond1000 benchmark:
    - Edit the param `RPQ_PAPER_HOME` in `scripts/benchmark_diamond_mdb.py`
    - The script needs to be executed as root user to clear the cache of the operating system:
        - `sudo su -`
    - change directory to `$RPQ_PAPER_HOME`
    - `bash scripts/benchmark_diamond_mdb.sh`


## Neo4J
- Install Neo4J python driver
    - `pip3 install neo4j`

- Download and extract the neo4j linux executable: https://neo4j.com/download-center/#community

- Set NEO4J_HOME with the path of the folder extracted
    - `export NEO4J_HOME=/path/to/neo4j/folder`

- Create the **diamond1000** database
    ```
    $NEO4J_HOME/bin/neo4j-admin import --database diamond1000 \
    --nodes $RPQ_PAPER_HOME/data/diamond1000_neo4j_nodes.csv \
    --relationships $RPQ_PAPER_HOME/data/diamond1000_neo4j_edges.csv
    ```

- Edit `$NEO4J_HOME/conf/neo4j.conf` adding the lines:
    ```
    dbms.transaction.timeout=1m
    dbms.default_database=diamond1000
    dbms.security.auth_enabled=false
    cypher.forbid_shortestpath_common_nodes=false
    ```

- Run the benchmark with diamond graph:
    - Start the neo4j server
        - `$NEO4J_HOME/bin/neo4j console`
    - Wait until the server is ready and run the benchmark in another terminal:
        - `python3 scripts/benchmark_diamond_neo4j.py`

    - After the benchmark is finished, kill the neo4j server with CTRL-C.

- To import WDBench and run the benchmark see https://github.com/MillenniumDB/WDBench and use `queries/wdbench_path_neo4j.txt` for the **ALL TRAILS** (default) semantic, and `queries/wdbench_path_neo4j_func.txt` for the **ALL SHORTEST PATHS** and **ANY SHORTEST PATH** semantics.

## SPARQL Systems
see https://github.com/MillenniumDB/WDBench and use `queries/wdbench_paths.txt` instead.