

var VALID_SKU_CHARS = [
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '_',
    'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q',      'R', 'S', 'T',
    'U', 'V', 'W',      'X', 'Y', 'Z'
];

function is_valid_sku_char(c)
{
    return VALID_SKU_CHARS.indexOf(c) > -1;
}

function search_for_iap_trial()
{
    console.log("Search for IAP Trial SKU");

    var result_count = mh_search("_TRIAL");
    if (!result_count) {
        console.log("No string ending with _TRIAL found\n");
        return -1;
    }

    var key = '';

    while (mh_result_next()) {
        var result = mh_result_get();
        // console.log(result);
        // console.log("read memory " + mh_integer_sub(result[0], "0x1a") + ' 0x30');

        // mh_memory_dump(mh_integer_sub(result[0], "0x1a"), 0x30);
        var bytes = mh_memory_read(mh_integer_sub(result[0], "0x1a"), 0x30);
        // console.log(bytes);

        bytes = Duktape.dec('hex', bytes);

        var apos = 0x1a;
        var bpos = 0x20;
        var i;

        for (i = bpos; i < 0x30; i++) {
            if (!is_valid_sku_char(String.fromCharCode([bytes[i]]))) {
                break;
            }
        }
        bpos = i;

        var found_length = 0;
        for (i = apos; i > 0; i--) {
            if (bytes[i] == (bpos - i - 1)) {
                found_length = bytes[i];
                break;
            } else if (!is_valid_sku_char(String.fromCharCode([bytes[i]]))) {
                break;
            }
        }
        apos = i + 1;

        // console.log(apos.toString(16) + ':' + bpos.toString(16));
        if (found_length) {
            for (i = apos; i < bpos; i++) {
                key += String.fromCharCode(bytes[i]);
            }

            console.log("found length   = " + found_length);
            console.log("Trial Key      = " + key);
            console.log("SKU Address    = " + mh_integer_add(mh_integer_sub(result[0], "0x1a"), apos.toString(16)));
            console.log("Length Address = " + mh_integer_add(mh_integer_sub(result[0], "0x1a"), (apos - 1).toString(16)));
            break;
        }
        // console.log(key);
    }

    mh_result_free();

    return key;
}


function search_for_all_iap_skus(prefix)
{
    console.log("Search for all IAP SKUs");

    var result_count = mh_search(prefix);
    if (!result_count) {
        console.log("No string beginning with "+prefix+" found\n");
        return -1;
    }

    var keys = [];

    while (mh_result_next()) {
        var result = mh_result_get();
        // console.log(result);
        // console.log("read memory " + mh_integer_sub(result[0], "0x1a") + ' 0x30');

        // mh_memory_dump(mh_integer_sub(result[0], "0x1"), 0x30);
        var bytes = mh_memory_read(mh_integer_sub(result[0], "0x1"), 0x30);
        // console.log(bytes);

        bytes = Duktape.dec('hex', bytes);

        var apos = 0x01;
        var bpos = prefix.length + 1;
        var i;

        for (i = bpos; i < 0x30; i++) {
            if (!is_valid_sku_char(String.fromCharCode([bytes[i]]))) {
                break;
            }
        }
        bpos = i;

        var found_length = 0;
        if (bytes[0] == bpos - 1) {
            found_length = bytes[0];
        }

        // console.log(apos.toString(16) + ':' + bpos.toString(16));
        if (found_length) {
            var key = '';
            for (i = apos; i < bpos; i++) {
                key += String.fromCharCode(bytes[i]);
            }

            // console.log("found length=" + found_length);
            console.log(key);
            keys.push(key);
        }
        // console.log(key);
    }

    mh_result_free();

    return keys;
}



var trial_key = search_for_iap_trial();
var iap_sku_prefix = trial_key.split('_')[0] + '_';
console.log("Prefix=" + iap_sku_prefix);
var all_iap_keys = search_for_all_iap_skus(iap_sku_prefix);

