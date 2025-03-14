# Cache-Simulator

## Simulation Details

The simulator requires several input parameters to run:
- **Cache size** (in bytes)
- **Associativity** (the number of cache lines in a set)
- **Cache policy** (either `fo` for FIFO or `lru` for Least Recently Used)
- **Block size** (in bytes)
- **Trace file** (the file containing memory access traces)

### Memory Access Trace Format

Each line in the trace file consists of two columns:
1. **Memory Access Type** (`R` for Read, `W` for Write)
2. **Memory Address** (a 48-bit address, e.g., `0x9cb3d40`)

## Cache Replacement Policies

### FIFO (First In, First Out)
- The cache evicts the block that was accessed first when space is needed.

### LRU (Least Recently Used)
- The cache evicts the least recently accessed block when space is needed.

---

## Running the Simulator
```bash
To run the one-level cache simulator (L1), use the following command format in first/:

./first <cache_size> <assoc:n> <cache_policy> <block_size> <trace_file>

Ex: ./first 32 assoc:2 fifo 4 trace1.txt 


To run the two-level cache simulator (L2), use the following command format in second/:

./second <L1_cache_size> <L1_assoc:n> <L1_cache_policy> <L1_block_size> <L2_cache_size> <L2_assoc:n> <L2_cache_policy> <trace_file>

Ex: ./second 32 assoc:2 fifo 4 64 assoc:16 lru trace2.txt
