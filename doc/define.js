

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
 * @return void
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