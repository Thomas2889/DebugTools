#ifdef DEBUG
#include "HeapMonitor.h"

#include <iostream>
#include <cstdio>
#include <unordered_set>


struct alloc_info
{
	alloc_info *last;
	alloc_info *next;
	int size;
	const char *file;
	int line;
};
alloc_info *last_alloc = NULL;
std::unordered_set<unsigned long> allocated_addresses;
FILE *logfile = NULL;

static unsigned char padding[4] = { 0x21, 0xd8, 0x1a, 0x7e };


void check_pad(alloc_info *info);
void *do_new(void *ptr, std::size_t count, const char *file, int line);
void *do_delete(void *p, const char *file, int line);


void setup_heap_monitor() {
	logfile = fopen("heap_monitor.log", "w+");
}

void check_heap_allocations() {
	alloc_info *info;
	int total_leakage = 0, total_leaks = 0;

	fprintf(logfile, "\n-------------------- Checking Heap --------------------\n\n");

	for (info = last_alloc; info; info = info->last) {
		fprintf(logfile, "[INFO] Unfreed memory 0x%p of %d bytes allocated at %s:%d\n",
			(char *)info + sizeof(alloc_info) + 4, info->size, info->file, info->line);

		check_pad(info);

		total_leakage += info->size;
		total_leaks += 1;
	}

	fprintf(logfile, "\n[INFO] %d leaks totalling %d bytes\n", total_leaks, total_leakage);

	fflush(logfile);
	fclose(logfile);
}

void *operator new(std::size_t count, const char *file, int line) {
	// allocate memory for alloc_info, padding and the actual info
	void *ptr = ::operator new(count + sizeof(alloc_info) + 8);
	return do_new(ptr, count, file, line);
}

void *operator new[](std::size_t count, const char *file, int line) {
	// allocate memory for alloc_info, padding and the actual info
	void *ptr = ::operator new[](count + sizeof(alloc_info) + 8);
	return do_new(ptr, count, file, line);
}

void *do_new(void *ptr, std::size_t count, const char *file, int line) {
	alloc_info *info = (alloc_info *)ptr; // ptr to alloc_info
	ptr = (char *)ptr + sizeof(alloc_info) + sizeof(padding); // move ptr forward for actual info

	char *begin_pad = (char *)ptr - sizeof(padding);
	char *end_pad = (char *)ptr + count;
	memcpy(begin_pad, padding, sizeof(padding));
	memcpy(end_pad, padding, sizeof(padding));

	info->last = last_alloc;
	if (last_alloc)
		last_alloc->next = info;
	last_alloc = info;
	info->next = nullptr;

	info->size = count;
	info->file = file;
	info->line = line;

	allocated_addresses.insert((unsigned long)info);

	fprintf(logfile, "[INFO] Allocation 0x%p of %d bytes at %s:%d\n", ptr, count, file, line);

	return ptr;
}

void operator delete(void *p, const char *file, int line) {
	p = do_delete(p, file, line);
	::operator delete(p);
}

void operator delete[](void *p, const char *file, int line) {
	p = do_delete(p, file, line);
	::operator delete[](p);
}

void *do_delete(void *p, const char *file, int line) {
	p = (char *)p - sizeof(alloc_info) - sizeof(padding); // move ptr backward for alloc_info
	alloc_info *info = (alloc_info *)p;

	if (allocated_addresses.find((unsigned long)info) == allocated_addresses.end()) {
		fprintf(logfile, "[ERROR] attempted to delete unallocated memory at %s:%d\n", file, line);
		return p;
	} else {
		if (!info->next) {
			last_alloc = info->last;
		} else {
			info->next->last = info->last;
		}
		if (info->last)
			info->last->next = info->next;

		fprintf(logfile, "[INFO] Deallocation 0x%p of %d bytes at %s:%d\n", p, info->size, file, line);
	}

	check_pad(info);

	return p;
}

void check_pad(alloc_info *info) {
	const char *ptr = (const char *)info;
	if (memcmp(ptr + sizeof(alloc_info), padding, sizeof(padding)) != 0)
		fprintf(logfile, "    [ERROR] pad check failed: underrun detected for allocation at %s:%d\n", info->file, info->line);
	if (memcmp(ptr + sizeof(alloc_info) + 4 + info->size, padding, sizeof(padding)) != 0)
		fprintf(logfile, "    [ERROR] pad check failed: overrun detected for allocation at %s:%d\n", info->file, info->line);
}
#endif
