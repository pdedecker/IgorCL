
__kernel void VectorAdd(__global float* arrayA, __global float* arrayB, __global float* arrayC) {
    int unitID = get_global_id(0);
    
    arrayC[unitID] = arrayA[unitID] + arrayB[unitID];
}
