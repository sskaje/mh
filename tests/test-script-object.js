
function test(pid) {
    var mh1 = new MH();
    mh1.open(pid);

    console.log("PID=" + mh1.get_pid());

    var addr = "0x0000000083ef9fe7";
    var size = 0x10;

    console.log("test memory dump");

    mh1.memory_dump(addr, size);

    // console.log("test memory write");
    // mh1.memory_write(addr, "aaabbbccc");

    var x;

    x = mh1.memory_read(addr, size);
    console.log(x);

    /*
    var byteArray = Duktape.dec('hex', x);
    for (var i = 0; i < byteArray.length; i ++) {
        console.log(byteArray[i].toString(16))
    }
    */

    // console.log("test memory write");
    // mh1.memory_write(addr, Duktape.enc('hex', "Hi mmm"));

    // x = mh1.memory_read(addr, size);
    // console.log(x);


    console.log("search 'Hello, world!'");
    var result_count = mh1.search("Hello, world!");

    console.log("result count: " + result_count);

    while (mh1.result_next()) {
        var result = mh1.result_get();
        console.log(result);

        mh1.memory_dump(result[0], 0x20);
    }


    mh1.close();

}

// global variable args;
console.log(args);

test(20655);

