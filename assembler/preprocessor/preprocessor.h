#ifndef preprocessor_h_
#define preprocessor_h_

static void create_preprocessed_file_copy(char* filename, char* tmp_file);
static void preprocessing_clean_file(char* tmp_file);
static void remove_comments_from_file(char* tmp_file);

void preprocessor(char* filename, char* tmp_file);

#endif
