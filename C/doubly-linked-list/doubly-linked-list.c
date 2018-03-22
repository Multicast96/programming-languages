#include<stdio.h>

typedef struct{
	struct element* next;
	int key;
	struct element *previous;
}element;


void print_list(element *head) {
	head = head->next;
	while (head != NULL){
		printf("%d->", head->key);
		head = head->next;
	}
	printf("NULL\n");
}

void push_front(element *head, int key) {
	element *tmp = (element*)malloc(sizeof(element));
	tmp->key = key;
	tmp->previous = head;
	tmp->next = head->next;
	
	if(head->next != NULL) ((element *)head->next)->previous = tmp;
	head->next = tmp;
}

void push_back(element *head, int key) {
	element *tmp = (element*)malloc(sizeof(element));
	tmp->key = key;
	tmp->next = NULL;

	while (head->next != NULL) head = head->next;

	head->next = tmp;
	tmp->previous = head;
}

void remove_key(element *head, int key) {
	element *tmp = head->next;

	while (tmp != NULL){
		if (tmp->key == key) {
			if (tmp->previous != NULL) ((element *)tmp->previous)->next = tmp->next;
			if (tmp->next != NULL) ((element *)tmp->next)->previous = tmp->previous;
			
			free(tmp);
			return;
		}
		tmp = tmp->next;
	}
}

int main() {
	element list = { NULL, 0 , NULL };
	push_front(&list, 10);
	push_front(&list, 20);
	push_front(&list, 30);
	push_back(&list, 5);
	push_back(&list, 1);
	print_list(&list);
	remove_key(&list, 30);
	print_list(&list);
	remove_key(&list, 1);
	print_list(&list);
	remove_key(&list, 10);
	remove_key(&list, 100);
	print_list(&list);
	return 0;
}