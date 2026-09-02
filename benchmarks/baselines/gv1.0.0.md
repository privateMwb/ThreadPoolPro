| Benchmark | Time | CPU | Iterations |
|---|---|---|---|
| invoke small (SBO) | 2.23 ns | 2.23 ns | 317354556 |
| invoke large (heap) | 1.87 ns | 1.87 ns | 374058426 |
| active/queued counts | 2.80 ns | 2.80 ns | 249895466 |
| idle thread count | 1.40 ns | 1.40 ns | 499695207 |
| paused/stopped/empty | 4.05 ns | 4.05 ns | 172969211 |
| detach single task (PulseThreadPool)/real_time | 224.42 ns | 134.83 ns | 2481351 |
| detach single task (oneTBB)/real_time | 186.96 ns | 186.82 ns | 3652953 |
| detach batch + drain (64 tasks) (PulseThreadPool)/real_time | 58843.24 ns | 35139.11 ns | 10000 |
| detach batch + drain (64 tasks) (oneTBB)/real_time | 4852.48 ns | 4847.25 ns | 144271 |
| enqueue + result (PulseThreadPool)/real_time | 916.08 ns | 898.19 ns | 711028 |
| enqueue + result (oneTBB)/real_time | 232.01 ns | 231.74 ns | 3023996 |
| pushBottom + popBottom (uncontended) | 28.65 ns | 28.65 ns | 24528651 |
| pushBottom (contended by steal)/real_time | 120.86 ns | 120.86 ns | 5723795 |
| construct + destroy (PulseThreadPool)/real_time | 12262.04 ns | 11622.19 ns | 57404 |
| construct + destroy (oneTBB)/real_time | 876.60 ns | 876.49 ns | 794635 |
| move construct (SBO) | 8.70 ns | 8.70 ns | 83355287 |
| move construct (heap) | 17.76 ns | 17.75 ns | 39428961 |
| pushBottom at backlog/backlog:0 | 66.38 ns | 66.20 ns | 10480928 |
| pushBottom at backlog/backlog:1024 | 64.12 ns | 64.11 ns | 10824628 |
| pushBottom at backlog/backlog:65536 | 64.12 ns | 64.12 ns | 10502204 |
| worker count sweep (PulseThreadPool)/workers:1/real_time | 65716.70 ns | 63666.47 ns | 10867 |
| worker count sweep (PulseThreadPool)/workers:2/real_time | 62030.48 ns | 52618.44 ns | 10534 |
| worker count sweep (PulseThreadPool)/workers:4/real_time | 169684.81 ns | 157675.16 ns | 4014 |
| worker count sweep (PulseThreadPool)/workers:8/real_time | 1403621.41 ns | 165752.85 ns | 472 |
| worker count sweep (PulseThreadPool)/workers:16/real_time | 621912.96 ns | 164082.48 ns | 903 |
| worker count sweep (PulseThreadPool)/workers:32/real_time | 263463.83 ns | 164187.70 ns | 3163 |
| worker count sweep (oneTBB)/workers:1/real_time | 7772.84 ns | 7771.65 ns | 90320 |
| worker count sweep (oneTBB)/workers:2/real_time | 32257.87 ns | 32253.03 ns | 21183 |
| worker count sweep (oneTBB)/workers:4/real_time | 17491.31 ns | 17483.76 ns | 39743 |
| worker count sweep (oneTBB)/workers:8/real_time | 17541.45 ns | 17532.73 ns | 39774 |
| worker count sweep (oneTBB)/workers:16/real_time | 17537.97 ns | 17524.12 ns | 39952 |
| worker count sweep (oneTBB)/workers:32/real_time | 17468.67 ns | 17454.22 ns | 39534 |
| detach (uncaught exception)/real_time | 99.96 ns | 56.08 ns | 11159015 |
| detach (caught exception) (PulseThreadPool)/real_time | 122.18 ns | 61.85 ns | 7455001 |
| detach (caught exception) (oneTBB)/real_time | 167.54 ns | 167.29 ns | 5407341 |
| pause + resume cycle/real_time | 524.14 ns | 523.64 ns | 1328819 |
