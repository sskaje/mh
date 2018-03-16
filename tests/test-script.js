function test() {

    var addr = "0x000060c000465402";
    var size = 0x10;

    console.log("test memory dump");

    mh_memory_dump(addr, size);

    // console.log("test memory write");
    // mh_memory_write(addr, "aaabbbccc");

    var x;

    x = mh_memory_read(addr, size);
    console.log(x);

    /*
    var byteArray = Duktape.dec('hex', x);
    for (var i = 0; i < byteArray.length; i ++) {
        console.log(byteArray[i].toString(16))
    }
    */

    // console.log("test memory write");
    // mh_memory_write(addr, Duktape.enc('hex', "Hi mmm"));

    // x = mh_memory_read(addr, size);
    // console.log(x);


    console.log("search 'Hello, world!'");
    var result_count = mh_search("Hello, world!");

    console.log("result count: " + result_count);

    while (mh_result_next()) {
        var result = mh_result_get();
        console.log(result);

        mh_memory_dump(result[0], 0x20);
    }
}

test();

