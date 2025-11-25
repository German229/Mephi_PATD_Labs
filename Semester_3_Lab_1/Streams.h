#ifndef STREAMS_H
#define STREAMS_H

#include "sequence.h"
#include <istream>
#include <ostream>
#include <fstream>
#include <functional>
#include <string>
#include <stdexcept>

template<typename T>
class ReadOnlyStream {
public:
    using Deserializer = std::function<bool(std::istream&, T&)>;

    // Stream from Sequence<T> (including LazySequence<T>)
    explicit ReadOnlyStream(Sequence<T>* seqSource)
        : type(SourceType::Sequence),
          seq(seqSource),
          in(nullptr),
          ownedStream(nullptr),
          deserializer(),
          position(0),
          endReached(false),
          opened(true) {}

    // Stream from existing std::istream (e.g. std::cin)
    ReadOnlyStream(std::istream& input, Deserializer d)
        : type(SourceType::IStream),
          seq(nullptr),
          in(&input),
          ownedStream(nullptr),
          deserializer(d),
          position(0),
          endReached(false),
          opened(true) {}

    // Stream from file
    ReadOnlyStream(const std::string& fileName, Deserializer d)
        : type(SourceType::IStream),
          seq(nullptr),
          in(nullptr),
          ownedStream(new std::ifstream(fileName)),
          deserializer(d),
          position(0),
          endReached(false),
          opened(false)
    {
        if (!ownedStream->is_open()) {
            delete ownedStream;
            ownedStream = nullptr;
            throw std::runtime_error("cannot open file");
        }
        in = ownedStream;
    }

    ~ReadOnlyStream() {
        Close();
    }

    void Open() {
        opened = true;
    }

    void Close() {
        if (ownedStream) {
            if (ownedStream->is_open()) {
                ownedStream->close();
            }
            delete ownedStream;
            ownedStream = nullptr;
        }
        opened = false;
    }

    bool IsEndOfStream() const {
        if (!opened) return true;
        if (type == SourceType::Sequence) {
            if (!seq) return true;
            return position >= static_cast<std::size_t>(seq->GetLength());
        } else {
            return endReached;
        }
    }

    bool IsCanSeek() const {
        return type == SourceType::Sequence && seq != nullptr;
    }

    bool IsCanGoBack() const {
        return IsCanSeek();
    }

    std::size_t GetPosition() const {
        return position;
    }

    void Seek(std::size_t index) {
        if (!IsCanSeek()) {
            throw std::runtime_error("seek is not supported for this stream");
        }
        if (!seq) {
            throw std::runtime_error("no sequence source");
        }
        if (index > static_cast<std::size_t>(seq->GetLength())) {
            index = static_cast<std::size_t>(seq->GetLength());
        }
        position = index;
    }

    T Read() {
        T value{};
        if (!TryRead(value)) {
            throw std::runtime_error("end of stream");
        }
        return value;
    }

    bool TryRead(T& value) {
        if (!opened) return false;

        if (type == SourceType::Sequence) {
            if (!seq) return false;
            if (position >= static_cast<std::size_t>(seq->GetLength())) {
                return false;
            }
            value = seq->Get(static_cast<int>(position));
            ++position;
            return true;
        } else {
            if (endReached || !in || !deserializer) {
                return false;
            }
            bool ok = deserializer(*in, value);
            if (!ok) {
                endReached = true;
                return false;
            }
            ++position;
            return true;
        }
    }

private:
    enum class SourceType {
        Sequence,
        IStream
    };

    SourceType type;
    Sequence<T>* seq;
    std::istream* in;
    std::ifstream* ownedStream;
    Deserializer deserializer;
    std::size_t position;
    bool endReached;
    bool opened;
};

// ------------------------ WriteOnlyStream ------------------------

template<typename T>
class WriteOnlyStream {
public:
    using Serializer = std::function<void(std::ostream&, const T&)>;

    // Stream to existing std::ostream (e.g. std::cout)
    WriteOnlyStream(std::ostream& output, Serializer s)
        : out(&output),
          ownedStream(nullptr),
          serializer(s),
          position(0),
          opened(true) {}

    // Stream to file
    WriteOnlyStream(const std::string& fileName, Serializer s)
        : out(nullptr),
          ownedStream(new std::ofstream(fileName)),
          serializer(s),
          position(0),
          opened(false)
    {
        if (!ownedStream->is_open()) {
            delete ownedStream;
            ownedStream = nullptr;
            throw std::runtime_error("cannot open file");
        }
        out = ownedStream;
        opened = true;
    }

    ~WriteOnlyStream() {
        Close();
    }

    void Open() {
        opened = true;
    }

    void Close() {
        if (ownedStream) {
            if (ownedStream->is_open()) {
                ownedStream->close();
            }
            delete ownedStream;
            ownedStream = nullptr;
        }
        opened = false;
    }

    std::size_t GetPosition() const {
        return position;
    }

    void Write(const T& value) {
        if (!opened || !out || !serializer) {
            throw std::runtime_error("stream is not open for writing");
        }
        serializer(*out, value);
        ++position;
    }

private:
    std::ostream* out;
    std::ofstream* ownedStream;
    Serializer serializer;
    std::size_t position;
    bool opened;
};

#endif