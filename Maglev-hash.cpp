#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct device {
	const char *name;
	int offset;
	int skip;
	int permutation[40010];
	int entry[40010];
	int broke[40010];
	struct device *next;
};

//--------------------- -Define---------------------------//
int M_min = 11;
int device_num = 4;
//--------------------------------------------------------//


//----------------------Function---------------------------//

void add_node(device **start, const char *name, int cnt);		//  add node in link-list  //
void print_list(device *node);									// print permutation table //
unsigned int offset(const char *str);						    //    calculate offset     //
unsigned int  skip(const char *data, int len);				    //     calculate skip      // 
void entry(device *node, device *del , int cnt);				//  generate entry table   //
void delete_node(device *start, const char *name);
void print_brk(device *node);

//--------------------------------------------------------//

int main() {


	device *head = NULL;
	device *del = NULL;

	/*add new device in code*/
	
	const char *device_name;

	device_name = "08:00:27:DC:53:9A";
	add_node(&head, device_name, 0);
	device_name = "08:00:27:CC:53:02";
	add_node(&head, device_name, 1);
	device_name = "08:00:27:AC:56:FF";
	add_node(&head, device_name, 2);
	device_name = "08:00:27:EE:78:FF";
	add_node(&head, device_name, 3);

	/*print list*/
	print_list(head);

	/*generate entry table*/
	entry(head, NULL ,device_num);


	const char *broken_node;

	del = head;

	/*delete broken node*/
	broken_node = "08:00:27:CC:53:02";
	delete_node(head, broken_node);
	
	print_brk(head);

}

void print_list(device *node)
{
	while (node != NULL) {
		printf("name = %s,offset = %d,skip = %d\n", node->name, node->offset, node->skip);
		for (int i = 0; i < M_min; i++) {
			printf("p[%d] = %d\n", i, node->permutation[i]);
		}

		node = node->next;
	}
}

void print_brk(device *node)
{
	while (node != NULL) {
		printf("name = %s,offset = %d,skip = %d\n", node->name, node->offset, node->skip);
		for (int i = 0; i < M_min; i++) {
			printf("p[%d] = %d\n", i, node->broke[i]);
		}

		node = node->next;
	}
}

void add_node(device **start, const char *name, int cnt)
{
	device *new_node = (device*)malloc(sizeof(device));
	new_node->name = name;
	new_node->offset = offset(name);
	new_node->skip = skip(name, 17);
	for (int i = 0; i < M_min; i++) {
		new_node->permutation[i] = (new_node->offset + i * new_node->skip) % M_min;
	}

	new_node->next = NULL;

	if (*start == NULL) {
		*start = new_node;
		return;
	}
	else {
		device *current;
		current = *start;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = new_node;
		return;
	}
}

unsigned int offset(const char *str)
{
	/*use DJB_HASH*/
	unsigned int hash = 5381;
	while (*str) {
		hash = ((hash << 5) + hash) + (*str++); /* times 33 */
	}
	hash &= ~(1 << 31); /* strip the highest bit */

	/*calculate offset*/
	hash = hash % M_min;
	return hash;
}


unsigned int  skip(const char *data, int len)
{
	unsigned int  h, k;

	h = 0 ^ len;

	while (len >= 4) {
		k = data[0];
		k |= data[1] << 8;
		k |= data[2] << 16;
		k |= data[3] << 24;

		k *= 0x5bd1e995;
		k ^= k >> 24;
		k *= 0x5bd1e995;

		h *= 0x5bd1e995;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch (len) {
	case 3:
		h ^= data[2] << 16;
	case 2:
		h ^= data[1] << 8;
	case 1:
		h ^= data[0];
		h *= 0x5bd1e995;
	}

	h ^= h >> 13;
	h *= 0x5bd1e995;
	h ^= h >> 15;

	h = h % (M_min - 1) + 1;

	return h;
}

void entry(device *node, device *del,int cnt)
{
	int index[100];
	int entry[40010];
	int entry_cnt = 0, index_cnt = 0;
	int test;
	device *head = node;

	for (int i = 0; i < cnt; i++) {
		index[i] = 0;
	}
	for (int i = 0; i < 40010; i++) {
		entry[i] = -1;
	}


	while (entry_cnt < M_min) {
		index_cnt = 0;
		while (node != NULL) {
			if (index[index_cnt] >= M_min) {
				printf("%d 's is out of blank\n", index_cnt);
			}
			else {
				test = node->permutation[index[index_cnt]];
				while (entry[test] != -1) {
					index[index_cnt] = index[index_cnt] + 1;
					test = node->permutation[index[index_cnt]];
				}
				entry[test] = index_cnt;
				node = node->next;
				index_cnt = index_cnt + 1;
				entry_cnt = entry_cnt + 1;
			}

			if (entry_cnt == M_min)
				break;
		}
		node = head;
	}

	node = head;

	if ( del == NULL) {
		while (node != NULL) {
			for (int i = 0; i < M_min; i++) {
				node->entry[i] = entry[i];
			}
			node = node->next;
		}

		node = head;
		int balance[5] = { 0 };

		printf("-----------------------------entry table---------------------------------------\n");

		for (int i = 0; i < M_min; i++) {
			printf("x[%d]:%d\n", i, node->entry[i]);
			++balance[node->entry[i]];
		}

		printf("--------------------------------------------------------------------------------\n");

		printf("%d %d %d %d\n", balance[0], balance[1], balance[2], balance[3]);
	}
	else {
		for (int i = 0; i < M_min; i++) {
			del->broke[i] = entry[i];
		}
	}

		
}



void delete_node(device *start, const char *name){
	
	device *head = (device*)malloc(sizeof(device));
	device *pre = (device*)malloc(sizeof(device));

	int brk_node = 0;

	head = start;
	pre = NULL;

	while (head != NULL) {
		if (head->name == name) {
			if (pre != NULL) {
				pre->next = head->next;
			}
			else {
				start = head->next;
				head = head->next;
			}
			
			break;
		}
		pre = head;
		head = head->next;
		brk_node++;
	}

	entry(start, head, device_num - 1);
	printf("-----------------------------broke table---------------------------------------\n");
	printf("/%d/\n", brk_node);
	

	int balance[5] = { 0 };

	for (int i = 0; i < M_min; i++) {
		
		if (head->broke[i] >= brk_node) {
			head->broke[i] = head->broke[i] + 1;
		}
		
		printf("brk[%d]:%d\n", i, head->broke[i]);
		++balance[head->broke[i]];
	}
	printf("%d %d %d %d\n", balance[0], balance[1], balance[2], balance[3]);
	printf("--------------------------------------------------------------------------------\n");

}




