project('esw-epoll', 'c', default_options: ['warning_level=3', 'werror=true'])
inc_dirs = include_directories('incl')
src_files = ['src/main.c', 'src/all.c', 'src/stdin.c', 'src/timer.c', 'src/tcp_server.c', 'src/isotp_server.c']
executable('epoll_server', src_files, include_directories: inc_dirs)
