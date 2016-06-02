/*************************************************************************\
 *                  Copyright (C) Michael Kerrisk, 2014.                   *
 *                                                                         *
 * This program is free software. You may use, modify, and redistribute it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation, either version 3 or (at your option) any      *
 * later version. This program is distributed without any warranty.  See   *
 * the file COPYING.gpl-v3 for details.                                    *
* \*************************************************************************/

#define _BSD_SOURCE     /* Get major() and minor() from <sys/types.h> */
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "file_perms.h"
#include "tlpi_hdr.h"
#include <grp.h>
#include <pwd.h>

static void
displayStatInfo(const struct stat *sb)
{
    struct passwd *pwd;
    pwd = getpwuid(sb->st_uid);
    struct group *grp;
    grp = getgrgid(sb->st_gid);
    printf("  Size: %lld           Blocks: %lld    IO Block: %ld ", (long long) sb->st_size,(long long) sb->st_blocks,(long) sb->st_blksize);

    switch (sb->st_mode & S_IFMT) {
    case S_IFREG:  printf("regular file\n");            break;
    case S_IFDIR:  printf("directory\n");               break;
    case S_IFCHR:  printf("character device\n");        break;
    case S_IFBLK:  printf("block device\n");            break;
    case S_IFLNK:  printf("symbolic (soft) link\n");    break;
    case S_IFIFO:  printf("FIFO or pipe\n");            break;
    case S_IFSOCK: printf("socket\n");                  break;
    default:       printf("unknown file type?\n");      break;
    }

    printf(" Device: %lxah/%ldd",
                (long) major(sb->st_dev)+1, (long) minor(sb->st_dev));

    printf("    Inode: %ld", (long) sb->st_ino);
    printf("    Links:   %ld\n", (long) sb->st_nlink);

    if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
        printf("    special bits set:     %s%s%s\n",
                (sb->st_mode & S_ISUID) ? "set-UID " : "04000",
                (sb->st_mode & S_ISGID) ? "set-GID " : "02000",
                (sb->st_mode & S_ISVTX) ? "sticky " : "01000");

    printf(" Access:  (%4o",
             sb->st_mode & 007777); 
    printf((S_ISDIR(sb->st_mode)) ? "d" : "-");
    printf("%s)", filePermStr(sb->st_mode, 0));

    printf("   UID:(%ld/%s)   GID:(%ld/%s)\n",
            (long) sb->st_uid, pwd->pw_name, (long) sb->st_gid, grp->gr_name);

    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode))
        printf("Device number (st_rdev):  major=%ld; minor=%ld\n",
                (long) major(sb->st_rdev), (long) minor(sb->st_rdev));
    char buf[200];
    strftime(buf,sizeof(buf),"Access:%Y-%m-%d %T %z\n", localtime(&sb->st_atime));
    printf(" %s",buf);
    strftime(buf,sizeof(buf),"Modify:%Y-%m-%d %T %z\n", localtime(&sb->st_mtime));
    printf(" %s",buf);
    strftime(buf,sizeof(buf),"Change:%Y-%m-%d %T %z\n", localtime(&sb->st_ctime));
    printf(" %s",buf);

}

int
main(int argc, char *argv[])
{
    struct stat sb;
    Boolean statLink;           /* True if "-l" specified (i.e., use lstat) */
    int fname;                  /* Location of filename argument in argv[] */

    statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
                                /* Simple parsing for "-l" */
    fname = statLink ? 2 : 1;

    if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s [-l] file\n"
                "        -l = use lstat() instead of stat()\n", argv[0]);

    if (statLink) {
        if (lstat(argv[fname], &sb) == -1)
            errExit("lstat");
    } else {
        if (stat(argv[fname], &sb) == -1)
            errExit("stat");
    }
    printf("  File: '%s'\n",argv[fname]);
    displayStatInfo(&sb);

    exit(EXIT_SUCCESS);
}
