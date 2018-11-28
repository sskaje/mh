

/**
 * write memory
 *
 * @param address
 * @param data
 * @return bool
 */
function mh_memory_write(address, data)
{

}
/**
 * read memory
 *
 * @param address
 * @param size
 * @return string
 */
function mh_memory_read(address, size)
{

}

/**
 * dump memory
 *
 * @param address
 * @param size
 * @return true
 */
function mh_memory_dump(address, size)
{

}

/**
 * search bytes
 *
 * @param needle
 * @returns {number}
 */
function mh_search(needle)
{

    return 0;
}

/**
 * update search
 *
 * @param needle
 * @returns {number}
 */
function mh_search_update(needle)
{

    return 0;
}

/**
 * get current result count
 *
 * @returns {number}
 */
function mh_result_count()
{
    return 0;
}

/**
 * get current result
 * ["address", "region_address", region_size]
 *
 * @returns {*[]}
 */
function mh_result_get()
{
    return ["address", "region_address", 0];
}

/**
 * move pointer to next
 * returns true if next element exists, otherwise false
 *
 * @returns {boolean}
 */
function mh_result_next()
{
    return true;
}

/**
 * remove node from result list by address
 *
 * @param address
 * @returns {boolean}
 */
function mh_result_remove(address)
{
    return true;
}

/**
 * reset result pointer
 *
 * @returns {boolean}
 */
function mh_result_reset()
{
    return true;
}

/**
 * free result set
 *
 * @returns {boolean}
 */
function mh_result_free()
{
    return true;
}

/**
 * Native function for int64 add
 *
 * @param str_a
 * @param str_b
 * @returns {*}
 */
function mh_integer_add(str_a, str_b)
{
    return "str_a + str_b";
}

/**
 * Native function for int64 sub
 *
 * @param str_a
 * @param str_b
 * @returns {*}
 */
function mh_integer_sub(str_a, str_b)
{
    return "str_a - str_b";
}

/**
 * class MH
 *
 * @constructor
 */

function MH() {
}
/**
 * Open task
 *
 * @param pid
 * @return bool
 */
MH.prototype.open = function (pid) {
    return true;
};
/**
 * Close task
 * @returns {boolean}
 */
MH.prototype.close = function () {
    return true;
};
/**
 * Get pid
 * @returns {number}
 */
MH.prototype.get_pid = function () {
    return 0;
};
/**
 * write memory
 *
 * @param address
 * @param data
 * @return bool
 */
MH.prototype.memory_write = function (address, data) {
    return true;
};
/**
 * read memory
 *
 * @param address
 * @param size
 * @return string
 */
MH.prototype.memory_read = function (address, size) {
    return "";
};
/**
 * dump memory
 *
 * @param address
 * @param size
 * @return true
 */
MH.prototype.memory_dump = function (address, size) {
    return true;
};
/**
 * search bytes
 *
 * @param needle
 * @returns {number}
 */
MH.prototype.search = function (needle) {
    return 0;
};

/**
 * update search
 *
 * @param needle
 * @returns {number}
 */
MH.prototype.search_update = function (needle) {
    return 0;
};
/**
 * get current result count
 *
 * @returns {number}
 */
MH.prototype.result_count = function () {
    return 0;
};
/**
 * reset result pointer
 *
 * @returns {boolean}
 */
MH.prototype.result_reset = function () {
    return true;
};
/**
 * get current result
 * ["address", "region_address", region_size]
 *
 * @returns {*[]}
 */
MH.prototype.result_get = function () {
    return ["", "", 0];
};
/**
 * move pointer to next
 * returns true if next element exists, otherwise false
 *
 * @returns {boolean}
 */
MH.prototype.result_next = function () {
    return true;
};
/**
 * free result set
 *
 * @returns {boolean}
 */
MH.prototype.result_free = function () {
    return true;
};
/**
 * remove node from result list by address
 *
 * @param address
 * @returns {boolean}
 */
MH.prototype.result_remove = function (address) {
    return true;
};

/**
 * global input variables
 * copied from c argc/argv
 *
 * @type {Array}
 */
var args = [];