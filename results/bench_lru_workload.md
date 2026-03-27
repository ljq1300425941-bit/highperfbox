# LRU Benchmark Results

## Default Cases

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_50_ks_1000_ops_100000_Uniform | 4679340 ns | 5.01% | 21.37M/s |
| cap_50_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 4318969 ns | 15.71% | 23.15M/s |
| cap_50_ks_1000_ops_100000_Zipf_za_120 | 2582357 ns | 66.63% | 38.72M/s |
| cap_100_ks_1000_ops_100000_Uniform | 4608385 ns | 10.01% | 21.70M/s |
| cap_100_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 3851635 ns | 31.28% | 25.96M/s |
| cap_100_ks_1000_ops_100000_Zipf_za_120 | 2401815 ns | 75.87% | 41.64M/s |
| cap_200_ks_1000_ops_100000_Uniform | 4229598 ns | 20.12% | 23.64M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 2573227 ns | 58.16% | 38.86M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_120 | 1655428 ns | 84.20% | 60.41M/s |
| cap_500_ks_1000_ops_100000_Uniform | 2859559 ns | 49.83% | 34.97M/s |
| cap_500_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 1373661 ns | 87.26% | 72.80M/s |
| cap_500_ks_1000_ops_100000_Zipf_za_120 | 1117680 ns | 93.61% | 89.47M/s |

## Key Space Sweep

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_200_ks_500_ops_100000_Uniform | 3280132 ns | 40.06% | 30.49M/s |
| cap_200_ks_500_ops_100000_Hotspot_hk_20_ha_80 | 1603003 ns | 84.26% | 62.38M/s |
| cap_200_ks_500_ops_100000_Zipf_za_120 | 1322623 ns | 89.85% | 75.61M/s |
| cap_200_ks_1000_ops_100000_Uniform | 4074311 ns | 20.12% | 24.54M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 2552485 ns | 58.16% | 39.18M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_120 | 1699189 ns | 84.20% | 58.85M/s |
| cap_200_ks_5000_ops_100000_Uniform | 4913291 ns | 4.11% | 20.35M/s |
| cap_200_ks_5000_ops_100000_Hotspot_hk_20_ha_80 | 4494530 ns | 12.87% | 22.25M/s |
| cap_200_ks_5000_ops_100000_Zipf_za_120 | 2277122 ns | 75.28% | 43.92M/s |

## Hotspot Intensity Sweep

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_200_ks_1000_ops_100000_Hotspot_hk_30_ha_70 | 4090549 ns | 33.39% | 24.45M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_20_ha_80 | 2724016 ns | 58.16% | 36.71M/s |
| cap_200_ks_1000_ops_100000_Hotspot_hk_10_ha_90 | 1352597 ns | 91.12% | 73.93M/s |

## Zipf Alpha Sweep

| Benchmark | CPU Time | HitRate | OpsPerSec |
|-----------|----------|---------|-----------|
| cap_200_ks_1000_ops_100000_Zipf_za_80 | 3228737 ns | 52.38% | 30.97M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_100 | 2314090 ns | 69.69% | 43.21M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_120 | 1738571 ns | 84.20% | 57.52M/s |
| cap_200_ks_1000_ops_100000_Zipf_za_150 | 1093609 ns | 95.42% | 91.44M/s |