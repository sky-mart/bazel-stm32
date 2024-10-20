#pragma once

namespace mart {

struct WavHeader
{
  enum Format : uint16_t {
    PCM         = 0x1,
    IEEE_FLOAT  = 0x3,
    ALAW        = 0x6,
    MULAW       = 0x7,
    EXTENSIBLE  = 0xFFFE
  };
  // WAV-формат начинается с RIFF-заголовка:

  // Содержит символы "RIFF" в ASCII кодировке
  // (0x52494646 в big-endian представлении)
  char chunkId[4];

  // 36 + subchunk2Size, или более точно:
  // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
  // Это оставшийся размер цепочки, начиная с этой позиции.
  // Иначе говоря, это размер файла - 8, то есть,
  // исключены поля chunkId и chunkSize.
  uint32_t chunkSize;

  // Содержит символы "WAVE"
  // (0x57415645 в big-endian представлении)
  char format[4];

  // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
  // Подцепочка "fmt " описывает формат звуковых данных:

  // Содержит символы "fmt "
  // (0x666d7420 в big-endian представлении)
  char subchunk1Id[4];

  // 16 для формата PCM.
  // Это оставшийся размер подцепочки, начиная с этой позиции.
  uint32_t subchunk1Size;

  // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
  // Для PCM = 1 (то есть, Линейное квантование).
  // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
  Format audioFormat;

  // Количество каналов. Моно = 1, Стерео = 2 и т.д.
  uint16_t numChannels;

  // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
  uint32_t sampleRate;

  // sampleRate * numChannels * bitsPerSample/8
  uint32_t byteRate;

  // numChannels * bitsPerSample/8
  // Количество байт для одного сэмпла, включая все каналы.
  uint16_t blockAlign;

  // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
  uint16_t bitsPerSample;

  // Подцепочка "data" содержит аудио-данные и их размер.

  // Содержит символы "data"
  // (0x64617461 в big-endian представлении)
  char subchunk2Id[4];

  // numSamples * numChannels * bitsPerSample/8
  // Количество байт в области данных.
  uint32_t subchunk2Size;

  // Далее следуют непосредственно Wav данные.

  uint16_t bytesPerSample() const { return bitsPerSample >> 3; }
};

}
