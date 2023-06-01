# Project build

MillenniumDB should be able to be built in any x86-64 linux distribution.
If you work on windows, you can use Windows Subsystem for Linux (WSL).

1. Install prerequisites to compile:

    - Boost Library version 1.71.0 or newer
    - Gcc version 8.1 or newer
    - Cmake version 3.10 or newer

    For distributions based on **Ubuntu 20.04 or newer** this can be done by running:
    - `sudo apt update`
    - `sudo apt install git g++ cmake libboost-all-dev`

     Other linux distributions may need to install the prerequisites differently. Some distributions might have repositories with too old versions and the project won't compile, in that case you'll need to manually install the appropriate versions.

2. Build the project:
    - `cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release && cmake --build build/Release/`

# Creating a database
The command to create a new database looks like this:
- `build/Release/bin/create_db [path/to/import_file] [path/to/new_database_folder]`

# Querying a database
We implement the typical a client/server model, so in order to query a database, you need to have a server running and then send queries to it.
## Run the server
- `build/Release/bin/server [path/to/database_folder]`

## Execute a query
- `build/Release/bin/query < [path/to/query_file]`