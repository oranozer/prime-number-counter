# prime-number-finder

## Compile

```bash
gcc -o prime main.c queue.c -pthread -lm
```
## Run
```bash
./prime -t 5 -q 5 -r 10 -m 200 -n 1000 -g 100
```

## Usage

[-t num_threads] [-q queue_size] [-r num_randoms] [-m lower_bound] [-n upper_bound] [-g generation_rate]
