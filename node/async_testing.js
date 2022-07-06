import bindings from 'bindings';
import {tests} from "./test_ressources.js";

const imagy = bindings('imagy');

const getTestData = (data) => {
    return [data.testFile, data.targetFile, data.targetHash];
}

const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.png);

const convertAsync = async () => {
    return imagy.convertAsync({
        'image': testFile,
        'outName': targetFile,
    });
};

console.log('Before');
//convertAsync().then((resolve) => console.log(resolve));
const blub = convertAsync( (resolve) => {
    console.log('yup');
    return resolve;
});

console.log(blub);
console.log('After');
