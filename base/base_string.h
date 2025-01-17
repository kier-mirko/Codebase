#ifndef BASE_STRING
#define BASE_STRING

#define str8_lit(s) (String8){.str = (U8 *)(s), .size = sizeof(s) - 1}
#define str8_lit_init(s) { (U8 *)(s), sizeof(s) - 1 }
#define str8_expand(s) s.str, s.size

// =============================================================================
// Unicode codepoint
typedef struct Codepoint Codepoint;
struct Codepoint {
  U32 codepoint;
  U8 size;
};

fn Codepoint utf8_decode(U8 *glyph_start);
fn Codepoint utf16_decode(U16 *glyph_start);
inline Codepoint utf32_decode(U32 *glyph_start);
fn U8 utf8_encode(U8 *res, Codepoint cp);
fn U8 utf16_encode(U16 *res, Codepoint cp);
inline U8 utf32_encode(U32 *res, Codepoint cp);

// =============================================================================
// UTF-8 string

typedef struct String8 String8;
struct String8 {
  U8 *str;
  U64 size;
};

typedef struct String8Node String8Node;
struct String8Node {
  String8Node *next;
  String8Node *prev;
  String8 string;
};

typedef struct String8List String8List;
struct String8List {
  String8Node *first;
  String8Node *last;
  USZ size;
};

fn String8 str8_list_join(Arena *arena, String8List strlist);
fn void str8_list_append(Arena *arena, String8List strlist, String8 other);

inline String8 str8(U8 *chars, USZ len);
inline String8 str8_from_cstr(char *chars);
inline String8 str8_from_date_time(Arena *arena, DateTime dt);
inline String8 str8_from_unix_time(Arena *arena, U64 unix_timestamp);

fn B32 str8_match(String8 s1, String8 s2);
fn B32 str8_match_cstr(String8 s, const char *cstr);
fn B32 cstr_match(char *s1, char *s2);

fn B32 str8_is_signed_int(String8 s);
fn B32 str8_is_int(String8 s);
fn B32 str8_is_float(String8 s);
inline B32 str8_is_number(String8 s);
fn I64 I64_from_str8(String8 s);
fn U64 U64_from_str8(String8 s);
fn F64 F64_from_str8(String8 s);

fn USZ str_hash(String8 s);

fn String8 str8_from_I64(Arena *arena, I64 n);
fn String8 str8_from_U64(Arena *arena, U64 n);
fn String8 str8_from_F64(Arena *arena, F64 n);

fn USZ cstr_len(char *chars);

fn String8 str8_format(Arena *arena, const char *fmt, ...);
fn String8 str8_format_va(Arena *arena, const char *fmt, va_list args);
fn String8 push_str8_copy(Arena *arena, String8 s);

fn String8 str8_prefix(String8 s, USZ end);
fn String8 str8_postfix(String8 s, USZ start);
fn String8 str8_substr(String8 s, USZ end);
fn String8 str8_range(String8 s, USZ start, USZ end);
fn B32 str8_ends_with(String8 s, char ch);
fn String8 str8_lcs(Arena *arena, String8 s1, String8 s2);

fn String8 str8_upper(Arena *arena, String8 s);
fn String8 str8_lower(Arena *arena, String8 s);
fn String8 str8_capitalize(Arena *arena, String8 s);

fn String8List str8_split(Arena *arena, String8 s, char ch);
fn USZ str8_find_first(String8 s, char ch);
fn USZ str8_find_first_substr(String8 s, String8 needle);
fn B32 str8_contains(String8 s, char ch);

fn B32 char_is_space(U8 ch);
fn B32 char_is_slash(U8 ch);
fn B32 char_is_upper(U8 ch);
fn B32 char_is_lower(U8 ch);
fn B32 char_is_digit(U8 ch);
fn B32 char_is_alpha(U8 ch);
fn B32 char_is_alphanumberic(U8 ch);

fn U8 char_to_upper(U8 ch);
fn U8 char_to_lower(U8 ch);
fn U8 str8_get_separator_from_os();

#if CPP
inline B32 operator==(String8 s1, String8 s2) {
  return str8_match(s1, s2);
}

inline B32 operator!=(String8 s1, String8 s2) {
  return !str8_match(s1, s2);
}
#endif

// =============================================================================
// Other UTF strings

typedef struct String16 String16;
struct String16{
  U16 *str;
  USZ size;
};

typedef struct String32 String32;
struct String32 {
  U32 *str;
  USZ size;
};

fn B32 str16_match(String16 s1, String16 s2);
fn B32 str32_match(String32 s1, String32 s2);

// =============================================================================
// UTF string conversion

fn String8 str8_from_str16(Arena *arena, String16 in);
fn String8 str8_from_str32(Arena *arena, String32 in);

fn String16 str16_from_str8(Arena *arena, String8 in);

fn String32 str32_from_str8(Arena *arena, String8 in);

// =============================================================================

#endif
