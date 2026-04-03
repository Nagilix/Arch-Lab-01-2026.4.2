#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

// ==========================================
// 实验 I: 矩阵每一列与向量的内积 (Cache优化)
// ==========================================

// 平凡算法：按列访问（Cache不友好）
void matrix_vector_naive(const vector<vector<int>>& mat, const vector<int>& vec, vector<int>& res, int n) {
    for (int j = 0; j < n; ++j) {
        int sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += mat[i][j] * vec[i]; // 内存跳跃访问，极易造成Cache Miss
        }
        res[j] = sum;
    }
}

// 优化算法：按行访问（Cache友好）
void matrix_vector_optimized(const vector<vector<int>>& mat, const vector<int>& vec, vector<int>& res, int n) {
    // 必须先清零
    for (int j = 0; j < n; ++j) res[j] = 0;
    
    // 交换内外层循环
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            res[j] += mat[i][j] * vec[i]; // mat[i][j] 在内存中是连续的
        }
    }
}

// ==========================================
// 实验 II: N个数求和 (指令级并行/超标量优化)
// ==========================================

// 平凡算法：链式累加
long long array_sum_naive(const vector<int>& arr, int n) {
    long long sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += arr[i]; // 存在严重的数据依赖，导致流水线阻塞
    }
    return sum;
}

// 优化算法：多路展开累加（利用超标量架构和多个ALU）
long long array_sum_optimized(const vector<int>& arr, int n) {
    long long sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int i = 0;
    // 4路循环展开
    for (; i <= n - 4; i += 4) {
        sum1 += arr[i];
        sum2 += arr[i + 1];
        sum3 += arr[i + 2];
        sum4 += arr[i + 3];
    }
    long long total = sum1 + sum2 + sum3 + sum4;
    // 处理剩余的尾部元素
    for (; i < n; ++i) {
        total += arr[i];
    }
    return total;
}

// ==========================================
// 测试与计时辅助函数
// ==========================================

void run_experiments() {
    cout << "================ 实验一：Cache优化 (矩阵列内积) ================" << endl;
    // 测试不同规模的N，观察跨越L1/L2 Cache大小时的性能断崖
    vector<int> sizes_mat = {512, 1024, 2048, 4096}; 
    for (int n : sizes_mat) {
        vector<vector<int>> mat(n, vector<int>(n, 1)); // 赋予固定值1以便验证
        vector<int> vec(n, 2);
        vector<int> res(n, 0);
        int repeat = 10; // 放大执行时间

        // 测试平凡算法
        auto start = high_resolution_clock::now();
        for(int r = 0; r < repeat; ++r) matrix_vector_naive(mat, vec, res, n);
        auto end = high_resolution_clock::now();
        double time_naive = duration_cast<microseconds>(end - start).count() / 1000.0 / repeat;

        // 测试优化算法
        start = high_resolution_clock::now();
        for(int r = 0; r < repeat; ++r) matrix_vector_optimized(mat, vec, res, n);
        end = high_resolution_clock::now();
        double time_opt = duration_cast<microseconds>(end - start).count() / 1000.0 / repeat;

        cout << "N=" << n << " | 平凡耗时: " << time_naive << " ms | 优化耗时: " << time_opt 
             << " ms | 加速比: " << time_naive / time_opt << endl;
    }

    cout << "\n================ 实验二：指令级并行 (数组求和) ================" << endl;
    vector<int> sizes_arr = {1000000, 10000000, 50000000}; 
    for (int n : sizes_arr) {
        vector<int> arr(n, 1);
        int repeat = 100; // 累加操作太快，需加大重复次数

        auto start = high_resolution_clock::now();
        for(int r = 0; r < repeat; ++r) array_sum_naive(arr, n);
        auto end = high_resolution_clock::now();
        double time_naive = duration_cast<microseconds>(end - start).count() / 1000.0 / repeat;

        start = high_resolution_clock::now();
        for(int r = 0; r < repeat; ++r) array_sum_optimized(arr, n);
        end = high_resolution_clock::now();
        double time_opt = duration_cast<microseconds>(end - start).count() / 1000.0 / repeat;

        cout << "N=" << n << " | 平凡耗时: " << time_naive << " ms | 优化耗时: " << time_opt 
             << " ms | 加速比: " << time_naive / time_opt << endl;
    }
}

int main() {
    run_experiments();
    return 0;
}