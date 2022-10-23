# 调用栈

# wrk压测

```shell
wrk -t8 -c200 -d60s --latency http://127.0.0.1:8888/
```

# perf

```shell
# 在root环境下运行
perf record -F 99 -p pid -g sleep 60
#
perf record -F 99 -p PID --call-graph dwarf sleep 10

# 
perf script > out.perf

#
git clone https://github.com/brendangregg/FlameGraph.git

./stackcollapse-perf.pl out.perf > out.folded
./flamegraph.pl out.folded > test.svg
# chrome 打开svg即可
```