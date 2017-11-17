#include <stdexcept>
#include <string>

#include <sndfile.hh>

#include "xtensor/xarray.hpp"
#include "xtensor/xeval.hpp"

#pragma cling load("sndfile")

namespace xt
{
    /**
     * Read a WAV or OGG file
     * This function reads a WAV file at ``filename``.
     *
     * @param filename File to load.
     *
     * @return tuple with (samplerate, xarray holding the data). The shape of the xarray
     *         is FRAMES x CHANNELS.
     *
     * @tparam T select type (default: short, 16bit)
     */
    template <class T = short>
    auto load_audio(std::string filename)
    {
        SndfileHandle file(filename);
        // testing for rawHandle because file not exist isn't handled otherwise
        if (!file || file.rawHandle() == nullptr)
        {
            throw std::runtime_error(std::string("load_wav: ") + file.strError());
        }
        auto result = xarray<T>::from_shape({(std::size_t) file.frames(), (std::size_t) file.channels()});
        file.read(result.raw_data(), (sf_count_t) result.size());
        return std::make_tuple(file.samplerate(), std::move(result));
    }

    /**
     * Save an xarray in WAV or OGG sound file format
     * Please consult the libsndfile documentation for more format flags.
     *
     * @param filename save under filename
     * @param data xarray/xexpression data to save
     * @param samplerate The samplerate of the data
     * @param format select format (see sndfile documentation).
     *               Default is 16 bit PCM WAV format.
     */
    template <class E>
    void dump_audio(std::string filename, const xexpression<E>& data, int samplerate,
                  int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16)
    {
        auto&& de = xt::eval(data.derived_cast());
        SndfileHandle file(filename, SFM_WRITE, format, (int) de.shape()[1], samplerate);
        // need to explicitly check the rawHandle otherwise permission errors etc. are not detected
        if (!file || file.rawHandle() == nullptr)
        {
            throw std::runtime_error(std::string("dump_wav: ") + file.strError());
        }
        file.write(de.raw_data(), (sf_count_t) de.size());
    }
}
