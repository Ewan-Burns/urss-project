#ifndef PLATTER_LINKED_LIST_H
#define PLATTER_LINKED_LIST_H
#include <cstddef> //needed for size_t
#include <iostream>
#include <iterator>
#include <functional>
template <typename T>
class chunk_list
{
    // Individual linked list element
    // You can put classes inside classes
    // Access as chunk_list::element (but it is private, so you can't!)
    struct element
    {
        // Store the templated datatype
        T data;
        element *prev = nullptr;
        element *next = nullptr;
        // constructor, copy item in
        element(const T &in) : data(in) {}
        element() {}
    };
    struct chunk
    {
        element *elements = nullptr;
        int count = 0, used = 0;
        chunk *next = nullptr;
        chunk *prev = nullptr;
        //constructor, allocate element based on count parameter
        chunk(size_t count)
        {
            elements = new element[count];
            this->count = count;
        }
        // Destructor - clear the list when the object is destroyed
        ~chunk()
        {
            if (!elements)
                return;
            delete[] elements;
        }
    };
    // Make iterator compatible with stl iterators
    struct iterator : public std::iterator<std::bidirectional_iterator_tag, typename chunk_list::element>
    {
        element *current = nullptr, *tail;
        // Allow access as *iterator (like STL iterators)
        T &operator*() const { return current->data; }
        // Allow comparison
        bool operator==(const iterator &other) const { return current == other.current; }
        bool operator!=(const iterator &other) const { return current != other.current; }
        // Move to next or previous item
        iterator &operator++()
        {
            current = current->next;
            return *this;
        }
        // Iterator has to store tail element so that we can -- from nullptr (i.e. end)
        iterator &operator--()
        {
            if (current == nullptr)
            {
                current = tail;
            }
            else
            {
                current = current->prev;
            }
            return *this;
        }
        iterator() {}
        iterator(element *c, element *t) : current(c), tail(t) {}
    };
    size_t elements = 0;
    element *head = nullptr, *tail = nullptr;
    chunk *head_chunk = nullptr, *tail_chunk = nullptr;

public:

    //Pack the chunks and relink the elements
    void pack_chunks()
    {
        if (!head_chunk)
            return;
        chunk *current = head_chunk;
        element *current_element = head, *prev_element=nullptr, *next_element=nullptr;
        while (current)
        {
            current->used = 0;
            current = current->next;
        }
        current = head_chunk;
        while (current_element)
        {
            if (current->used == current->count)
            {
                current = current->next;
            }
            next_element = current_element->next;
            current->elements[current->used] = *current_element;
            current->elements[current->used].prev=nullptr;
            current->elements[current->used].next=nullptr;
            current->used++;
            current_element = next_element;
        }
        //Delete all unused chunks
        current = head_chunk;
        chunk *next_chunk = nullptr;
        while (current)
        {
            next_chunk = current->next;
            if (current->used == 0)
            {
                if (current->prev)
                {
                    current->prev->next = current->next;
                }
                else
                {
                    head_chunk = current->next;
                }
                if (current->next)
                {
                    current->next->prev = current->prev;
                }
                else
                {
                    tail_chunk = current->prev;
                }
                delete current;
            }
            current = next_chunk;
        }
        //Now relink the elements
        head = &head_chunk->elements[0];
        head->prev=nullptr;
        current = head_chunk;
        size_t npart=1;
        prev_element = nullptr;
        int u=0;
        while(current)
        {
            for (u=0; u<current->used; u++)
            {
                current_element = &current->elements[u];
                current_element->prev = prev_element;
                if (prev_element)
                {
                    prev_element->next = current_element;
                }
                prev_element = current_element;
            }
            current = current->next;
        }
        tail = prev_element;       
    }

    //Add chunk
    void add_chunk()
    {
        chunk *new_chunk = new chunk(100);
        if (!head_chunk)
        {
            head_chunk = new_chunk;
            tail_chunk = head_chunk;
            return;
        }
        tail_chunk->next = new_chunk;
        new_chunk->prev = tail_chunk;
        tail_chunk = new_chunk;
    }

    // Create a new element
    element *create_element(const T &in)
    {
        if(!head_chunk)
        {
            add_chunk();
        }
        if (tail_chunk->used == tail_chunk->count)
        {
            add_chunk();
        }
        element *new_element = &tail_chunk->elements[tail_chunk->used];
        new_element->data = in;
        tail_chunk->used++;
        return new_element;
    }
    // Add at end
    void push_back(const T &in)
    {
        // If tail is null then no items so set up head and tail to be the new item
        if (!tail)
        {
            head = create_element(in);
            tail = head;
            return;
        }
        // Create the new item
        tail->next = create_element(in);
        // Set the new items "previous" link to the current last item
        tail->next->prev = tail;
        // Set the new last item to be the new item
        tail = tail->next;
        elements++;
    }
    // Add at the beginning
    void push_front(const T &in)
    {
        if (!head)
        {
            head = create_element(in);
            tail = head;
            return;
        }
        head->prev = create_element(in);
        head->prev->next = head;
        head = head->prev;
        elements++;
    }
    size_t size() { return elements; }

		size_t count(){element* current=head;
			size_t ct=0; while (current){ct++;current=current->next;}
			return ct;}

    iterator begin()
    {
        iterator i(head, tail);
        return i;
    }
    iterator end()
    {
        iterator i(nullptr, tail);
        return i;
    }

    // Function to sanity check the list
    // Only needed for debugging
    void check()
    {
        element *current;
        current = head;
        int index;
        while (current)
        {
            if (current->prev == nullptr && current != head)
                std::cout << "Invalid null prev\n";
            if (current->next == nullptr && current != tail)
                std::cout << "Invalid null next\n";
            current = current->next;
        }
    }

		void check_count(){
      element *current;
			current = head;
			size_t npart=0;
			while(current){
				npart++;current=current->next;
			}
			std::cout << "COUNT IS " << npart << "\n";
		}

    // Insert function comparable to that in std::list and std::vector
    template <typename otherit>
    iterator insert(iterator position, otherit first, otherit last)
    {
        element *current, *orig_next;
        if (position.current == nullptr)
        {
            current = tail;
        }
        else
        {
            current = position.current->prev;
        }

        orig_next = current->next;
        otherit it;
        int add = 0;
        for (it = first; it != last; ++it)
        {
            current->next = create_element(*it);
            current->next->prev = current;
            current = current->next;
            elements++;
            add++;
        }
        if (orig_next)
        {
            orig_next->prev = current;
            current->next = orig_next;
        }
        else
        {
            tail = current;
        }
        return iterator(orig_next, tail);
    }

    // Destructor - clear the list when the object is destroyed
    ~chunk_list()
    {
        clear();
    }

    // Access an element
    T &operator[](size_t index)
    {
        element *current = head;
        for (size_t i = 0; i < index; i++)
        {
            current = current->next;
        }
        return current->data;
    }

    //Clear the chunks
    void clear()
    {
				chunk *current, *next;
				current=head_chunk;
				while(current){
					next=current->next;
					delete current;
					current=next;
				}
        
    }

    iterator erase(iterator pos)
    {
        bool is_head = (pos.current->prev == nullptr);
        bool is_tail = (pos.current->next == nullptr);
        // Yes, this is horrible
        // If this isn't the first element then we have to
        // Get the previous and set the next pointer to
        // the current item's next
        if (!is_head)
        {
            pos.current->prev->next = pos.current->next;
        }
        else
        {
            this->head = pos.current->next;
            pos.current->next->prev = nullptr;
        }
        // If this isn't the last element then we have to
        // Get the next and set the prev pointer to
        // the current item's prev
        if (!is_tail)
        {
            pos.current->next->prev = pos.current->prev;
        }
        else
        {
            this->tail = pos.current->prev;
            pos.current->prev->next = nullptr;
        }
        iterator i(pos.current->next, tail);
        elements--;
        return i;
    }

    iterator erase(iterator start, iterator end)
    {
        iterator current;
        for (current = start; current != end;)
        {
            current = erase(current);
        }
				pack_chunks();
        return current;
    }
};

namespace std
{
    template <typename T>
    size_t erase_if(chunk_list<T> &list, std::function<bool(T &)> pred)
    {
        for (auto it = list.begin(); it != list.end();)
        {
            if (pred(*it))
            {
                it = list.erase(it);
            }
            else
            {
                ++it;
            }
        }
        return 0;
    }
};
#endif


