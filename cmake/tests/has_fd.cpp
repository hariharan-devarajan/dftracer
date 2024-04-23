#include <unistd.h>  // fsync
#include <fstream>
#include <type_traits>
#include <cstdlib> // EXIT_SUCCESS

template <typename _CharT, typename _Traits>
int fsync_ofstream (std::basic_ofstream<_CharT, _Traits>& os)
{
    class my_filebuf : public std::basic_filebuf<_CharT>
    {
       public:
        int handle ()
        {
            return this->_M_file.fd ();
        }
    };

    if (os.is_open ()) {
        os.flush ();
        return fsync (static_cast<my_filebuf&> (*os.rdbuf ()).handle ());
    }

    return EBADF;
}

template <typename _CharT, typename _Traits>
int fsync_fstream (std::basic_fstream<_CharT, _Traits>& os)
{
    class my_filebuf : public std::basic_filebuf<_CharT>
    {
       public:
        int handle ()
        {
            return this->_M_file.fd ();
        }
    };

    if (os.is_open ()) {
        os.flush ();
        return fsync (static_cast<my_filebuf&> (*os.rdbuf ()).handle ());
    }

    return EBADF;
}

int main ()
{
    int rc = 0;

    std::fstream os ("test_fsync_fstream", std::fstream::out);
    os << "test_fsync_fstream";
    rc = fsync_fstream (os);
    if (rc != 0) return EXIT_FAILURE;

    std::ofstream ofs ("test_fsync_ofstream", std::ofstream::out);
    ofs << "test_fsync_ofstream";
    rc = fsync_ofstream (ofs);
    if (rc != 0) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}