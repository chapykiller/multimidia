import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

public class Wavparser {
    static void parseWave(File file)
    throws IOException {
        FileInputStream in = null;
        try {
            in = new FileInputStream(file);
            byte[] bytes = new byte[4];

            // read first 4 bytes
            // should be RIFF descriptor
            if(in.read(bytes) < 0) {
                return;
            }

            printDescriptor(bytes);

            // first subchunk will always be at byte 12
            // there is no other dependable constant
            in.skip(8);

            for(;;) {
                // read each chunk descriptor
                if(in.read(bytes) < 0) {
                    break;
                }

                printDescriptor(bytes);

                // read chunk length
                if(in.read(bytes) < 0) {
                    break;
                }

                // skip the length of this chunk
                // next bytes should be another descriptor or EOF
                in.skip(
                      (bytes[0] & 0xFF)
                    | (bytes[1] & 0xFF) << 8
                    | (bytes[2] & 0xFF) << 16
                    | (bytes[3] & 0xFF) << 24
                );
            }

            System.out.println("end of file");

        } finally {
            if(in != null) {
                in.close();
            }
        }
    }

    static void printDescriptor(byte[] bytes)
    throws IOException {
         System.out.println(
             "found '" + new String(bytes, "US-ASCII") + "' descriptor"
         );
    }

    public static void main(String args[]) {
        try{
            File file = new File(args[0]);
            parseWave(file);
        }
        catch(IOException e) {
        }
    }
}
