var addon = require('bindings')('imagy')

console.log('This should be eight:', addon.add(3, 5))
