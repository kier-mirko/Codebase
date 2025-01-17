#ifndef BASE_THREAD_CTX_H
#define BASE_THREAD_CTX_H

typedef struct TCTX TCTX;
struct TCTX {
  Arena *arenas[2];
};

Arena *tctx_get_scratch(Arena **conflicts, usize count);

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)

#endif //BASE_THREAD_CTX_H
