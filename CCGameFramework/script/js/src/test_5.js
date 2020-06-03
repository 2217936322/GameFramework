console.log(new RegExp("ab").test("abc"));
console.log(/\d+/.test("abc123"));
console.log(JSON.stringify({})); // '{}'
console.log(JSON.stringify(true)); // 'true'
console.log(JSON.stringify("foo")); // '"foo"'
console.log(JSON.stringify([1, "false", false])); // '[1,"false",false]'
console.log(JSON.stringify({ x: 5 })); // '{"x":5}'
console.log(JSON.stringify({ x: 5, y: 6 })); // "{"x":5,"y":6}"
console.log(JSON.stringify([new Number(1), new String("false"), new Boolean(false)])); // '[1,"false",false]'
console.log(JSON.stringify({ x: undefined, y: Object })); // '{}'
console.log(JSON.stringify([undefined, Object])); // '[null,null]'
var obj = {
    foo: 'foo',
    toJSON: function(key) {
        return key + 'bar';
    }
};
JSON.stringify(obj); // '"bar"'
JSON.stringify({ x: obj }); // '{"x":"xbar"}'