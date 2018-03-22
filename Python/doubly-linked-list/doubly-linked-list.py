class Element:
    def __init__(self, key, next, previous):
        self.key = key
        self.next = next
        self.previous = previous


class List:
    def __init__(self):
        self.head = Element(0, None, None)

    def __str__(self):
        tmp = self.head.next
        object_string = ""
        while tmp is not None:
            object_string += str(tmp.key) + "->"
            tmp = tmp.next
        object_string += "None"
        return object_string

    def push_front(self, key):
        tmp = Element(key, self.head.next, self.head)
        if self.head.next is not None:
            self.head.next.previous = tmp
        self.head.next = tmp

    def push_back(self, key):
        tmp = self.head
        while tmp.next is not None:
            tmp = tmp.next
        tmp.next = Element(key, None, tmp)

    def remove_key(self, key):
        tmp = self.head
        while tmp is not None:
            if tmp.key == key:
                if tmp.previous is not None:
                    tmp.previous.next = tmp.next
                if tmp.next is not None:
                    tmp.next.previous = tmp.previous

                del tmp
                return
            tmp = tmp.next


list = List()
list.push_front(30)
list.push_front(20)
list.push_front(10)
list.push_back(1)
list.push_back(2)
list.push_back(3)
print(list)
list.remove_key(10)
print(list)
list.remove_key(3)
print(list)
list.remove_key(1)
print(list)

