#include <iostream> 
#include <cassert> 
#include <cstdlib> 
#include <cstring> 

class cpa_vector
{
    private: 
        int* p_arr; 
        std::size_t size; 

    public: 
        class iterator; 
        typedef std::size_t size_type; 

        cpa_vector(): p_arr(nullptr), size(0) {} 
        
        cpa_vector(const cpa_vector& other)
        {
            size = other.size; 
            p_arr = new int[size]; 
            memcpy((void*)p_arr, (void*)other.p_arr, size*sizeof(int)); 
        }

        cpa_vector(size_type _size) : p_arr(new int[_size]), size(_size)
        {
            memset((void*)p_arr, 0, _size * sizeof(int)); 
        }

        cpa_vector(size_type _size, int element): p_arr(new int[_size]), size(_size)
        {
            for(size_type i = 0; i < _size; ++i)
                p_arr[i] = element; 
        }

        cpa_vector(iterator iter_1, iterator iter_2)
        {
            std::size_t len = iter_2.p - iter_1.p; 
            std::cout << "len=" << len << std::endl; 
            p_arr = new int[len]; 
            size = len; 
            int*p = iter_1.p; 
            for(size_type i = 0; i < len; ++i)
                p_arr[i] = *p++; 
        }

        ~cpa_vector() {
            delete[] p_arr; 
        }

        void push_back(int new_element)
        {
            size += 1; 
            p_arr = (int*)realloc(p_arr, (size)*sizeof(int)); 
            p_arr[size-1] = new_element; 
        }

        size_type get_size() { return size; }

        class iterator 
        {
            friend class cpa_vector; 
            int* p; 

            public: 
                iterator() : p(nullptr) {} 
                iterator(int* _p) : p(_p) {} 

                iterator operator+(size_type k)
                {
                    return iterator(p+k); 
                }

                iterator operator-(size_type k)
                {
                    return iterator(p-k); 
                }

                iterator operator++(){
                    p += 1;
                    return *this;  
                }

                iterator operator--(){
                    p -= 1; 
                    return *this; 
                }

                int operator*(){
                    return *p; 
                }

                bool operator==(const iterator& other){
                    return p == other.p; 
                }

                bool operator!=(const iterator& other){
                    return p != other.p; 
                }
        }; 

        iterator begin() { return iterator(p_arr); }
        iterator end() { return iterator(p_arr + size); }

        int operator[](size_type i)
        {
            assert(i >= 0 && i < size); 
            return *(p_arr + i); 
        }

        void push_front(int element)
        {
            size = size + 1; 
            p_arr = (int*)realloc(p_arr, size * sizeof(int)); 
            for(std::size_t i = size - 2; i > -1; --i)
                p_arr[i+1] = p_arr[i]; 
            p_arr[0] = element; 
        }

        int pop_back()
        {
            int ret = p_arr[size-1]; 
            size = size - 1; 
            p_arr = (int*)realloc(p_arr, size * sizeof(int)); 
            return ret; 
        }

        iterator insert(iterator iter, int n, int val)
        {
            std::size_t iter_index = iter.p - p_arr; 
            std::size_t old_size = size; 

            size = size + n; 
            p_arr = (int*)realloc(p_arr, size * sizeof(int)); 
            for(std::size_t i = old_size - 1; i > iter_index - 1; i--)
                p_arr[i+n] = p_arr[i]; 
            for(std::size_t i = 0; i < n; ++i)
                p_arr[iter_index + i] = val; 
            
            return iterator(p_arr + iter_index); 
        }

        iterator insert(iterator iter, iterator b_iter, iterator e_iter)
        {
            std::size_t n = e_iter.p - b_iter.p; 
            std::size_t iter_index = iter.p - p_arr; 
            std::size_t old_size = size; 
            size = size + n; 
            p_arr = (int*)realloc(p_arr, size * sizeof(int)); 
            for(std::size_t i = old_size - 1; i > iter_index - 1; i--)
                p_arr[i+n] = p_arr[i]; 
            for(std::size_t i = 0; i < n; ++i)
                p_arr[iter_index + i] = *(b_iter.p + i);
            return iterator(p_arr + iter_index) ; 
        }

        iterator erase(iterator iter)
        {
            std::size_t iter_index = iter.p - p_arr; 
            
        }

        iterator erase(iterator b_iter, iterator e_iter)
        {

        }

        void clear()
        {
            delete[] p_arr; 
            p_arr = 0; 
            size = 0; 
        }
}; 

int main(void)
{
    cpa_vector v(10, 48); 

    std::cout << "By Index" << std::endl; 

    for(cpa_vector::size_type i = 0; i < v.get_size(); ++i)
        std::cout << "v[" << i << "]:" << v[i] << std::endl; 

    std::cout << "By Iterator" << std::endl; 
    for(cpa_vector::iterator iter = v.begin(); 
        iter != v.end(); 
        ++iter)
        std::cout << "*iter=" << *iter << std::endl; 

    cpa_vector::iterator iter_b = v.begin() + 1; 
    cpa_vector::iterator iter_e = v.begin() + 6; 
    cpa_vector v1(iter_b, iter_e); 

    std::cout << "Using index:" << std::endl; 
    for(cpa_vector::size_type i = 0; i != v1.get_size(); ++i)
        std::cout << "v1[" << i << "]:" << v1[i] << std::endl; 

    std::cout << "Using iterator:" << std::endl; 
    for(cpa_vector::iterator iter = v1.begin(); iter != v1.end(); ++iter)
        std::cout << "*iter = " << *iter << std::endl;

    cpa_vector::iterator iter_1 = v.begin() + 3; 
    iter_1 = v.insert(iter_1, 4, 100); 
    std::cout << "By Index" << std::endl; 
    for(cpa_vector::size_type i = 0; i < v.get_size(); ++i)
        std::cout << "v[" << i << "]:" << v[i] << std::endl; 

    cpa_vector v3(8, 300); 
    iter_1 = v.begin() + 8; 
    cpa_vector::iterator iter_b1 = v3.begin() + 2; 
    cpa_vector::iterator iter_e1 = v3.begin() + 6; 
    iter_1 = v.insert(iter_1, iter_b1, iter_e1); 
    std::cout << "By Index" << std::endl; 
    for(cpa_vector::size_type i = 0; i < v.get_size(); ++i)
        std::cout << "v[" << i << "]:" << v[i] << std::endl; 

    return 0; 
}

// linked 