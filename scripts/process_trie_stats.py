import sys

with open(sys.argv[1], 'r') as results_file, \
     open(sys.argv[2], 'r') as cache_info_file:

    results = results_file.read().strip().split('\n')[1:]
    cache_info = cache_info_file.read().split('\n')

    index = 0
    for results_line in results:
        cache_info_line = cache_info[index]
        index += 1

        trie_stats = [x for x in cache_info_line.split(';') if x.strip() != '']
        total_time = 0
        total_size = 0
        for trie_stat in trie_stats:
            prefix_lvl_str, data_lvl_str, time_str = trie_stat.split(',')

            prefix_lvl = int(prefix_lvl_str)
            data_lvl   = int(data_lvl_str)
            time       = float(time_str)

            total_size += (12*prefix_lvl) + (8*data_lvl) # Bytes
            total_time += time
        total_in_ms = int(total_time / 1_000_000) # nanoseconds to milliseconds
        print(f'{results_line},{total_size},{total_in_ms}')
