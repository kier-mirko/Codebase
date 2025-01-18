#ifndef BASE_TLS_H
#define BASE_TLS_H

typedef struct {
  Arena *arenas[2];
} TlsContext;

fn Arena *tlsGetScratch(Arena **conflicts, usize count);

#define ScratchBegin(conflicts, count) tmpBegin(tlsGetScratch((conflicts), (count)))
#define ScratchEnd(scratch) tmpEnd((scratch))

#endif
