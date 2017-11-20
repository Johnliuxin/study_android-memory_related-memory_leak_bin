/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <cutils/properties.h>
#include <media/MemoryLeakTrackUtil.h>

#define MB (1024 * 1024)

using namespace android;

int main(int argc, char *argv[])
{
    uint32_t per_inc_size = 10; //Mega-Byte
	uint32_t total_size = 100; //Mega-Byte
	uint32_t sleep_per_inc = 1; //Second
	uint32_t max_retry_count = 5; //Retry count when alloc failed

    opterr = 0;
    do {
        int c;

        c = getopt(argc, argv, "t:p:s:r:");

        if (c == EOF) {
            break;
        }

        switch (c) {
		case 't':
			total_size = atoi(optarg);
			break;
        case 'p':
            per_inc_size = atoi(optarg);
			break;
		case 's':
            sleep_per_inc = atoi(optarg);
			break;
		case 'r':
            max_retry_count = atoi(optarg);
			break;
        case '?':
            fprintf(stderr, "usage: %s [t num] [p num] [s num] [r num]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } while (1);

    if(argc > optind + 1) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("total_size %u, per_inc_size %u, sleep_per_inc %u, max_retry_count %u\n", total_size, per_inc_size, sleep_per_inc, max_retry_count);

	uint32_t i = 1;
	uint32_t retry_count = 1;
	time_t rawtime;
	struct tm* timeinfo;
	char t_buf[80];
	for(; i <= total_size/per_inc_size; i++) {
		char *buffer = (char*)malloc(per_inc_size * MB);
		if(!buffer) {
			printf("failed to alloc %d'th buf after allocated %d * %dM buf. retry ...\n", i, (i-1), per_inc_size);
			retry_count++;
			if(retry_count > max_retry_count)
				break;

			continue;
		}
		memset(buffer, '-', per_inc_size * MB);
		retry_count = 1;

		time(&rawtime);
		timeinfo = localtime (&rawtime);
		strftime (t_buf,80,"[%F %X] :", timeinfo);
		printf("%s total allocated %dM memory\n", t_buf, i * per_inc_size);

		sleep(sleep_per_inc);
	}

	int32_t debugMalloc = 0;
	char property_val[PROPERTY_VALUE_MAX];
	if(property_get("libc.debug.malloc", property_val, "0")) {
		debugMalloc = atoi(property_val);
	}
	
	if(debugMalloc == 1) {
		printf("Done\n");
		printf("-----------------------------------------------\n");
		printf("libc.debug.malloc == 1, now dumpMemoryAddresses\n");
		dumpMemoryAddresses(STDOUT_FILENO);
	} else if(debugMalloc == 10) {
		printf("Program will exit normal\n");
	} else {
		printf("Done & block, use Ctrl+C to kill the program\n");
		while(1);
	}

    return 0;
}
