# result

➜  Class3_7_thread_pool git:(main) ✗ time ./a.out 15
总计发布任务数: 1000000
总计运行线程数: 15
总结接收结果数: 1000000
线程池总计处理任务数: 1000000
./a.out 15  6.20s user 20.79s system 252% cpu 10.704 total

➜  Class3_7_thread_pool git:(main) ✗ time ./a.out 1 
总计发布任务数: 1000000
总计运行线程数: 1
总结接收结果数: 1000000
线程池总计处理任务数: 1000000
./a.out 1  18.37s user 128.16s system 109% cpu 2:14.18 total

➜  Class3_7_thread_pool git:(main) ✗ 

# 比较

15线程相比1线程，性能提升了 

$$ \frac{(128.16s - 20.79s)}{128.16s} * 100\% \approx 83.78\% $$
