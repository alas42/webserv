# ft_containers

/********************************************************************\
|************************* RULES ************************************|
\*******************************************************************/

Namespaces will aways be ft
The containers will be tested using ft::<container>
Respect of the structore of reference (the container)
If it's not canonical, don't make it canonical
C++98 not C++11

The main has to be bigger and test more things

!one binary with only my containers!
!one binary with the stl ones!

using std::allocator
justify the inner structures for each container (so I have to be logical ok)

Iterators has to be implemented too [A]
AND
    . iterators_traits [B]
    . reverse_iterators [C]
    . enable_if [D]
    . is_integral [E]
    . equal / lexicograhical comparisons [F]
    . std::pair [G]
    . std::make_pair [H]

/*
** EVERYTHING ELSE THAT DOES NOT APPEAR ON CPPREFENCE.COM HAS TO BE PRIVATE OR PROTECTED
*/

KEYWORD FRIEND ALLOWED FOR NON-MEMBER OVERLOADS (like << )

Now, the subject is:
A: vector
B: map
C: stack -> will use the vector class I made as base class
BONUS: (won't do)
    D: SET (Black-Red Tree or whatnot)


TO TEST :
/*
** github.com/Mazoise/FT_CONTAINERS_TESTER
*/

So basically, Have to write some code, hpp files, Template classes
What do I have to write and when can I just inherit something ? Dont know
Do I have to rewrite iterators and iterators_traits ? maybe, this is hell

/****************************************************\
|***************** RESEARCH**************************|
\***************************************************/

[A]

"std::iterator is the base class provided to simplify definitions of the required types for iterators."
Yeah to simplify, my ass

It has FIVE template paramaters.
1. A Category of iterator, it can one of FIVE categories
2. A Type of value that can be obtained by deferencing an iterator like *it gives 5 in case of an int
3. A Distance that is used to know the distance (?) between iterators (iterations ?)
4. A Pointer to the type iterated over *
5. A Reference to the type iterated over. &

THE FIVE CATEGORIES OF ITERATORS
(by the way, these are "empty_type" whatever that means ans are stucts)
1. struct input_iterator_tag 
2. struct output_iterator_tag
3. forward_iterator_tag: public input_iterator_tag
4. struct bidirectional_iterator_tag: public forward_iterator_tag
5. struct random_access_iterator_tag: public bidirectional_iterator_tag

One of these five categries has to be typedefed in every iterators. It indicated the "most specific category that the iterator is in".
They carry information used to select the most efficient algorithm that has to be used.

[B]

THEN, there is what is called iterators_traits
-> It provides a uniform interface for the iterators.
(makes possible to implement the algorithms for iterators)
It has to have FIVE (again) member types, variables (? have to check what it means)
1. difference type
2. value_type
3. pointer (can be declared void)
4. reference
5.  iterator_category 

So what do we learned ? jack s***

[C]
reverse_iterators

For a reverse iterator r constructed from an iterator i, the relationship &*r == &*(i-1) is always true
It has exactly the same template parameters as the iterator listed up here |

[D]
enable_if <bool B, class T = void>
BASICALLY what I can understand with my limited brain is that :
    the boolean determines what happens with the second template paramater
    if true = T has a type
    if false = void is the type (it has then no type)
By the way, this is not existing in c++98 but we have to do it in c++98. The more I read, the more fun it gets

[E]
is_integral
CHECK if template T is an integral type and returns true if it is a:
    bool, char, char16_t, char32_t, wchar_t, short, int, long, long long
returns false if not.

[F]
Yeah, I guess I know what that thing is but without any more info I don't even know what to check

[G]
std::pair is a class template to store two objects as one. OK ok, hear me out now.
What the heck ? Do we have to create a tuple class just to do it or ???
Because and I quote "a pair is specific case of a std::tuple with two elements"
Yeah, so since tuple are a c++11 thing, I will say we have to code it too...

[H]
std::make_pair
create a std::pair object ? then it just calls std::pair and returns it... why tho ?
why not calll std::pair directly ?

Hilf mir Gott, die sind verrueckt

WHAT IS AN ALLOCATOR ? 
It is an object that has strategies to access access and address values, to allocate and deallocate memory AND to construct and destruct objects.
the std::allocator class template is the one by default for all std containers. 
have to #include <memory> meybe to use it.

WHAT IS AN ITERATOR ?
"The reason that STL containers and algorithms work so well together, is that they know nothing of each other — Alex Stepanov"

In C++, every iterator is treated as a pointer
"Due to different types of iterator categories, C++ defines the iterator traits which a programmer needs to specify if he/she wants the best optimization when STL algorithm is used."
"in C++98, iterator traits are defined with typedef typename sintaxis"

Allocator does only allocate space as for new allocate and instanciate or create elements
We do not want that so we have to uses allocator's methods

Allocators method that we have to use :

pointer allocate(size_type n, const void * hint = 0) allocates n* sizeof(T) bytes of unititialized storage
    returns a poitner to the first element of an array whose elements have not been constructed yet
void deallocate(T * p, std::size_t n) deallocates storage pointed by p 
    [wich must be a pointer obtained by an earlier call to allocate]

max_size (returns the largest supported allocation size)
void construct(pointer p, const_reference val) constructs an object in allocated storage pointed to by p
destroy(pointer p) - destructs an object pointed by p in allocated storage

To learn a frigging lot about allocators : https://www.drdobbs.com/the-standard-librarian-what-are-allocato/184403759

To learn about iterators with a slide-show : http://www.cse.unsw.edu.au/~cs6771/16s2/lectures/lec08-1.pdf
AND : https://web.stanford.edu/class/cs107l/handouts/04-Custom-Iterators.pdf

---> THE QUESTION IS : WHY USE an allocator on ft::map when there is no reserve function in place ?

But, the rest in not very difficult.
Use allocator to allocate and construct the nodes.
Et voila.
Does the pair needs to be allocated ? No, well I don't think so anyway.
The bidirectionnal iterator is a mystery to me.
Does he has access to the root Node ?
I understand how begin() and end() should work but not what map::iterator++ will do
    Maybe it should look into the redblacktree and look for the next key, i don't know.
    I will try it tomorrow

Map sources: RedBlackTrees and Nodes
https://www.geeksforgeeks.org/red-black-tree-set-1-introduction-2/
https://github.com/dbjnbnrj/redblacktrees/blob/master/rb_node.h

I have to be more logical.

The redblacktree is it just a tool to handle nodes OR is it the ONLY WAY a map can access to a NODE ?
If it is the caase, I have to overload every iterator functions in redblacktree too.
Bit heavy, ain't it ?
Okay, so it is "just" a tool, what now ?
-> iterator have to access to the the contents of the nodes. should they have a redblacktree in it ?
-> Nope, if I have to make it right, they point to a pair OR I have to specialise the iterator.
-> Messy and actually I can't even if I want, the map iterator, a bidirectionnal one has to point over
    value_type (ft::pair<const Key, T>).
damn, how then ?

MORE LOGICAL DAM..T
Actually, a iterator doesn't has to access the rbtree. Just to two functions of node 
1. predecessor
2. successor

A newly instanciated iterator is useless.
It will be assigned the begin() or end() of the map or sonething like that
There is no problem anymore, right ?
I guess huh.
Next, on the plate we have :
    .Actually implement end() to test everything we just did
    .Do the rest of the functions like insert and stuff
    .Substract new and delete with the allocator's functions
    .Do Compare (value_compare) friend class
    .SFINAE : Enable_if and is_integral
    . AND TEST THE SHIT OUT OF IT