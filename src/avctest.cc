#include "AVHandler.h"
#include <cmath>

#define FRAMES 255

int output_avi(std::string fn, std::string codec) 
{
    AVHandler *av = new AVHandler();

    av->set_filename(fn);
    av->set_height(100);
    av->set_width(300);
    av->set_codec(codec);

    int err = av->setup_write();
    if (err != 0) {
        delete av;
        return err;
    }

    std::cout << "starting..." << std::endl;
    for (int i = 0; i < FRAMES; i++) {
        std::cout << "\r" << i << std::flush;
        av->draw_background((unsigned char)((1+sin(double(i)/1000*2*M_PI))*255/2),
                            (unsigned char)((1+cos(double(i)/1000*2*M_PI))*255/2), 200);
        av->write_frame();
    }

    delete av;
    return 0;
}

int main() {
#if defined(HAVE_THREADS)
    std::cout << "Warning: threads are active" << std::endl;
#endif

    std::cout << "Built for libavcodec " << LIBAVCODEC_BUILD;
    std::cout << " (using version " << avcodec_build() << ")" << std::endl;

    if (output_avi("testc1.avi", "wmv1") != 0)
        std::cout << "Error writing testc1.avi" << std::endl;
    if (output_avi("testc2.avi", "wmv1") != 0)
        std::cout << "Error writing testc2.avi" << std::endl;

    AVHandler av = AVHandler();
    av.set_filename("testc2.avi");
    av.setup_read();
    for (unsigned int i = 1; i < av.get_total_frames()+1; i++) {
        std::cout << "Reading frame " << i << "...\r" << std::flush;
        av.read_frame(i);
    }
   
    return 0;
}
