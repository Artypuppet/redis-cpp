#include <string>
#include <memory>

enum CommandTypes {
    PING,
    ECHO
};

enum DataTypes {
    // Simple data types
    SIMPLE_STRING,
    SIMPLE_INTEGER,
    SIMPLE_DOUBLE,
    SIMPLE_NULL,
    SIMPLE_BOOL,
    SIMPLE_BIGNUM,

    // Bulk data types
    BULK_STRING,
    BULK_VERB, // Verbatim string
    BULK_ERRORS, // Can be used only in a response

    // Aggregate data types
    AGGR_ARRAY,
    AGGR_MAP,
    AGGR_SET,
    AGGR_PUSH
};


class ParseTask {
public:
    int tidx; // Index in the task array in the Command
    int pos; // Where its reading in the input
    int len; // How much it has read
    ParseTask* parent; // Parent task. Raw ptr is fine as memory mgmt. will be taken care of by the vector
    DataTypes type;
};