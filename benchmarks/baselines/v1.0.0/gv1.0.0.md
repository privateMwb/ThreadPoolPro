| Benchmark | Time | CPU | Iterations |
|---|---|---|---|
| invoke small (SBO) | 2.15 ns | 2.15 ns | 331461796 |
| invoke large (heap) | 2.11 ns | 2.11 ns | 331552478 |
| active/queued counts | 3.18 ns | 3.18 ns | 220913065 |
| idle thread count | 1.58 ns | 1.58 ns | 441906244 |
| paused/stopped/empty | 4.57 ns | 4.57 ns | 153130958 |
| detach single task (PulseThreadPool)/real_time | 427.93 ns | 361.36 ns | 1999497 |
| detach single task (oneTBB)/real_time | 195.30 ns | 195.18 ns | 3374306 |
| detach batch + drain (64 tasks) (PulseThreadPool)/real_time | 62710.05 ns | 58209.32 ns | 11750 |
| detach batch + drain (64 tasks) (oneTBB)/real_time | 4343.88 ns | 4337.74 ns | 161611 |
| enqueue + result (PulseThreadPool)/real_time | 1207.38 ns | 1146.55 ns | 564373 |
| enqueue + result (oneTBB)/real_time | 215.97 ns | 215.86 ns | 3249839 |
| pushBottom + popBottom (uncontended) | 29.64 ns | 29.64 ns | 23657479 |
| pushBottom (contended by steal)/real_time | 123.88 ns | 123.88 ns | 5556015 |
| construct + destroy (PulseThreadPool)/real_time | 13550.46 ns | 12526.22 ns | 54501 |
| construct + destroy (oneTBB)/real_time | 829.77 ns | 829.67 ns | 843371 |
| move construct (SBO) | 9.18 ns | 9.17 ns | 76492537 |
| move construct (heap) | 17.96 ns | 17.96 ns | 39030949 |
| pushBottom at backlog/backlog:0 | 69.86 ns | 69.65 ns | 9718503 |
| pushBottom at backlog/backlog:1024 | 68.12 ns | 68.12 ns | 10978092 |
| pushBottom at backlog/backlog:65536 | 68.06 ns | 68.06 ns | 9973132 |
| worker count sweep (PulseThreadPool)/workers:1/real_time | 151852.56 ns | 151301.49 ns | 4303 |
| worker count sweep (PulseThreadPool)/workers:2/real_time | 73960.77 ns | 68014.67 ns | 10670 |
| worker count sweep (PulseThreadPool)/workers:4/real_time | 213195.58 ns | 208208.86 ns | 2976 |
| worker count sweep (PulseThreadPool)/workers:8/real_time | 983009.32 ns | 233312.26 ns | 615 |
| worker count sweep (PulseThreadPool)/workers:16/real_time | 514907.15 ns | 218566.84 ns | 1351 |
| worker count sweep (PulseThreadPool)/workers:32/real_time | 318887.79 ns | 216565.43 ns | 2278 |
| worker count sweep (oneTBB)/workers:1/real_time | 7998.80 ns | 7997.71 ns | 87336 |
| worker count sweep (oneTBB)/workers:2/real_time | 34082.92 ns | 34080.50 ns | 20806 |
| worker count sweep (oneTBB)/workers:4/real_time | 16080.64 ns | 16078.84 ns | 43078 |
| worker count sweep (oneTBB)/workers:8/real_time | 16267.82 ns | 16266.29 ns | 41713 |
| worker count sweep (oneTBB)/workers:16/real_time | 16464.28 ns | 16460.99 ns | 43077 |
| worker count sweep (oneTBB)/workers:32/real_time | 16317.22 ns | 16304.75 ns | 43454 |
| detach (uncaught exception)/real_time | 138.76 ns | 74.45 ns | 6483963 |
| detach (caught exception) (PulseThreadPool)/real_time | 161.82 ns | 83.94 ns | 6175991 |
| detach (caught exception) (oneTBB)/real_time | 181.37 ns | 181.24 ns | 4697167 |
| pause + resume cycle/real_time | 693.43 ns | 692.70 ns | 1046963 |
