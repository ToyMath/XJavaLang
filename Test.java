public class Test {
    public static void main(String[] args) {
        int i;
        i = 0;
        int j;
        j = 0;
        int k;
        k = 0;
        while (k < 10000) {
          while (j < 10000) {
            while (i < 10000) {
              i = i + 1;
            }
            j = j + 1;
          }
          k = k + 1;
        }

        System.out.println(i);
    }
}
