#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <cstddef> //needed for size_t
#include <iostream>
#include <iterator>
#include <functional>
template<typename T>
class basic_linked_list{
	//Individual linked list element
	//You can put classes inside classes
	//Access as basic_linked_list::element (but it is private, so you can't!)
  struct element{
		//Store the templated datatype
		T data;
		element *prev=nullptr;
		element *next=nullptr;
		//constructor, copy item in
		element(const T &in):data(in){}
	};
	//Make iterator compatible with stl iterators
	struct iterator:public std::iterator<std::bidirectional_iterator_tag
                                        , typename basic_linked_list::element>
	{
		element *current = nullptr, *tail;
		//Allow access as *iterator (like STL iterators)
		T& operator*()const{return current->data;}
		//Allow comparison
		bool operator==(const iterator& other)const{return current==other.current;}
		bool operator!=(const iterator& other)const{return current!=other.current;}
		//Move to next or previous item
		iterator& operator++(){current = current->next;return *this;}
		//Iterator has to store tail element so that we can -- from nullptr (i.e. end)
		iterator& operator--(){if (current==nullptr) {current=tail;}else{current = current->prev;}return *this;}
		iterator(){}
		iterator(element *c, element *t):current(c),tail(t){}
	};
	size_t elements=0;
	element *head=nullptr, *tail=nullptr;
public:
	//Add at end
	void push_back(const T& in){
		//If tail is null then no items so set up head and tail to be the new item
		if(!tail){
			head = new element(in);
			tail=head;
			return;
		}
		//Create the new item
		tail->next = new element(in);
		//Set the new items "previous" link to the current last item
		tail->next->prev = tail;
		//Set the new last item to be the new item
		tail = tail->next;
		elements++;
	}
	//Add at the beginning
	void push_front(const T& in){
		if (!head){
			head = new element(in);
			tail = head;
			return;
		}
		head->prev = new element(in);
		head->prev->next = head;
		head = head->prev;
		elements++;
	}
	size_t size(){return elements;}

	iterator begin(){iterator i(head,tail); return i;}
	iterator end(){iterator i(nullptr, tail); return i;}

//Function to sanity check the list
//Only needed for debugging
	void check(){
    element *current;
		current=head;
		int index;
		while(current){
			if (current->prev==nullptr && current!=head) std::cout << "Invalid null prev\n";
			if (current->next==nullptr && current!=tail) std::cout << "Invalid null next\n";
			current=current->next;
		}
	}

//Insert function comparable to that in std::list and std::vector
	template <typename otherit>
	iterator insert(iterator position, otherit first, otherit last){
		element *current, *orig_next;
		if (position.current==nullptr){
			current = tail;
		} else {
			current = position.current->prev;
		}

		orig_next=current->next;
		otherit it;
		int add=0;
		for (it=first; it!=last;++it){
			current->next = new element(*it);
			current->next->prev=current;
			current = current->next;
			elements++;
			add++;
		}
		if (orig_next){
			orig_next->prev = current;
			current->next = orig_next;
		} else {
			tail = current;
		}
		return iterator(orig_next,tail);
	}

  //Destructor - clear the list when the object is destroyed
  ~basic_linked_list(){
		clear();
	}

  //Clear the elements of the list
	//Frees the memory
	void clear(){
    element *current=head, *next;
    while(current){
      next=current->next;
      delete current;
      current = next;
    }
	}

	iterator erase(iterator pos){
		bool is_head = (pos.current->prev==nullptr);
		bool is_tail = (pos.current->next==nullptr);
		//Yes, this is horrible
		//If this isn't the first element then we have to
		//Get the previous and set the next pointer to 
		//the current item's next
		if (!is_head){
			pos.current->prev->next = pos.current->next;
		} else {
			this->head = pos.current->next;
			pos.current->next->prev=nullptr;
		}
    //If this isn't the last element then we have to
    //Get the next and set the prev pointer to 
    //the current item's prev
		if (!is_tail){
			pos.current->next->prev = pos.current->prev;
		} else {
			this->tail = pos.current->prev;
			pos.current->prev->next=nullptr;
		}
		iterator i(pos.current->next,tail);
		delete pos.current;
		elements --;
		return i;
	}

	iterator erase(iterator start, iterator end){
		iterator current;
		for (current = start;current!=end;){
			current = erase(current);
		}
		return current;
	}

};

namespace std{
	template <typename T>
		size_t erase_if(basic_linked_list<T> &list, std::function<bool(T&)> pred){
			for(auto it=list.begin();it!=list.end();){
				if (pred(*it)) {
					it=list.erase(it);
				} else {
					++it;
				}
			}
			return 0;
		}
};
#endif
