#ifndef __REDISCPP_CONTEXT__
#define __REDISCPP_CONTEXT__

#include <list>
#include <map>

template<typename ContextType>
class RediscppContext
{
public:
    using context_type = typename ContextType;
    typedef void *contextid_type;

    bool Pop(contextid_type context_id, context_type *context) {
        index_iterator_type index_iter = m_indexes.find(context_id);
        
        if(index_iter == m_indexes.end()) {
            return false;
        }

        node_iterator_type &node_iter = index_iter->second;

        if(context) {
            *context = std::move(*node_iter);
        }

        m_nodes.erase(node_iter);
        m_indexes.erase(context_id);

        return true;
    }

    contextid_type Push(context_type &&context) {
        node_iterator_type node_iter = m_nodes.insert( m_nodes.end(), context );
        contextid_type context_id = &(*node_iter);
        m_indexes[context_id] = std::move(node_iter);
        return context_id;
    }

private:
    std::list<context_type> m_nodes;
    using node_iterator_type = typename std::list<context_type>::iterator;
    std::map<contextid_type, node_iterator_type> m_indexes;
    using index_iterator_type = typename std::map<contextid_type, node_iterator_type>::iterator;
};


#endif

