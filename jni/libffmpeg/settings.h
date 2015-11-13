#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	int loop_filter_flag;
}Decode_flags;

extern Decode_flags decode_flags;

void set_loop_filter(int loop_filter_flag);

#ifdef __cplusplus
}
#endif