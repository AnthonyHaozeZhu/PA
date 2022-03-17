……此处省略替换的头文件
using namespace std;
int main()
{
    int a, b, i, t;
    a = 0;
    b = 100;
    i = 1;
    cout << a << endl;
    cout << b << endl;
    while(i < (15))
    {
        t = b;
        b = a +b ;
        cout << b << endl;
        a = t;
        i = i + 1;
    }
    return 0;
}