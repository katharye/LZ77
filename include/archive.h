#ifndef ARCHIVE_H
#define ARCHIVE_H

void ResetCoder(void);
void ArchiveFiles(char *archive_name, int file_count, char *filenames[]);
void ExtractFiles(char *archive_name, char *target_file);
void ListFiles(char *archive_name);

#endif // ARCHIVE_H
