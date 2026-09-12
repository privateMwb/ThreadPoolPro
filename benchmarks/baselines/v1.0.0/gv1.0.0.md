| Benchmark | Time | CPU | Iterations |
|---|---|---|---|
| invoke small | 2.11 ns | 2.11 ns | 325683707 |
| invoke large | 2.11 ns | 2.11 ns | 331783877 |
| active/queued | 3.16 ns | 3.16 ns | 221367286 |
| idle count | 1.58 ns | 1.58 ns | 442723955 |
| paused/stopped | 4.57 ns | 4.57 ns | 153169111 |
| detach 1 (PTP)/real_time | 567.80 ns | 501.64 ns | 1783597 |
| detach 1 (TBB)/real_time | 199.31 ns | 199.01 ns | 3370833 |
| detach batch64 (PTP)/real_time | 60674.17 ns | 57049.18 ns | 11572 |
| detach batch64 (TBB)/real_time | 4414.94 ns | 4400.05 ns | 154643 |
| enqueue (PTP)/real_time | 1224.55 ns | 1198.52 ns | 591940 |
| enqueue (TBB)/real_time | 216.98 ns | 216.93 ns | 3216734 |
| push/pop | 30.19 ns | 30.19 ns | 23402474 |
| push (steal)/real_time | 147.42 ns | 147.42 ns | 4867496 |
| ctor/dtor (PTP)/real_time | 12914.18 ns | 12023.33 ns | 44090 |
| ctor/dtor (TBB)/real_time | 824.94 ns | 824.80 ns | 846283 |
| move (SBO) | 9.15 ns | 9.15 ns | 76550688 |
| move (heap) | 17.96 ns | 17.96 ns | 38961344 |
| push@backlog/backlog:0 | 67.07 ns | 67.07 ns | 10048912 |
| push@backlog/backlog:1024 | 67.92 ns | 67.90 ns | 9896474 |
| push@backlog/backlog:65536 | 68.03 ns | 68.02 ns | 9475244 |
| workers (PTP)/workers:1/real_time | 160462.84 ns | 159945.16 ns | 4396 |
| workers (PTP)/workers:2/real_time | 71931.24 ns | 64693.63 ns | 10485 |
| workers (PTP)/workers:4/real_time | 233561.57 ns | 220655.89 ns | 2966 |
| workers (PTP)/workers:8/real_time | 943428.22 ns | 254972.07 ns | 553 |
| workers (PTP)/workers:16/real_time | 525625.44 ns | 234377.56 ns | 1256 |
| workers (PTP)/workers:32/real_time | 321657.47 ns | 221506.11 ns | 1773 |
| workers (TBB)/workers:1/real_time | 7992.50 ns | 7990.62 ns | 87613 |
| workers (TBB)/workers:2/real_time | 36001.32 ns | 35996.50 ns | 19650 |
| workers (TBB)/workers:4/real_time | 16902.69 ns | 16831.33 ns | 41214 |
| workers (TBB)/workers:8/real_time | 16874.84 ns | 16824.67 ns | 41557 |
| workers (TBB)/workers:16/real_time | 16784.04 ns | 16756.10 ns | 41843 |
| workers (TBB)/workers:32/real_time | 17024.50 ns | 17001.92 ns | 40868 |
| detach uncaught/real_time | 125.42 ns | 68.65 ns | 5490765 |
| detach caught (PTP)/real_time | 142.54 ns | 76.56 ns | 4637614 |
| detach caught (TBB)/real_time | 170.61 ns | 170.52 ns | 4685028 |
| pause/resume/real_time | 700.02 ns | 699.47 ns | 866216 |
