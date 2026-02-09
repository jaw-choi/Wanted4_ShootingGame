#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Util.h"

namespace Wanted
{
	Renderer::Frame::Frame(int bufferCount)
	{
		// 배열 생성 및 초기화.
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount);
	}

	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}

	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		// 2차원 배열로 다루는 1차원 배열을 순회하면서
		// 빈 문자(' ')를 설정.
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				// 배열 인덱스 구하기.
				const int index = (y * width) + x;

				// 글자 값 및 속성 설정.
				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' ';
				info.Attributes = 0;

				// 그리기 우선순위 초기화.
				sortingOrderArray[index] = -1;
			}
		}
	}

	// -------------- Frame -------------- //

	// 정적 변수 초기화.
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		instance = this;

		// 프레임 객체 생성.
		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);

		// 프레임 초기화.
		frame->Clear(screenSize);

		// 이중 버퍼 객체 생성 및 초기화.
		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		// 활성화 버퍼 설정.
		Present();
	}

	Renderer::~Renderer()
	{
		SafeDelete(frame);
		for (ScreenBuffer*& buffer : screenBuffers)
		{
			SafeDelete(buffer);
		}
	}

	void Renderer::Draw()
	{
		// 화면 지우기.
		Clear();

		// 전제조건: 레벨의 모든 액터가 렌더러에 Submit을 완료.
		// 렌더큐 순회하면서 프레임 채우기.
		for (const RenderCommand& command : renderQueue)
		{
		    if (!command.text || command.length <= 0) // length 체크로 변경
		    {
			continue;
		    }

		    // Y축 클리핑
		    if (command.position.y < 0 || command.position.y >= screenSize.y)
		    {
			continue;
		    }

		    // [수정] strlen 대신 미리 계산된 command.length 사용
		    const int length = command.length;

		    const int startX = command.position.x;
		    const int endX = command.position.x + length - 1;

		    // X축 클리핑 (완전히 벗어난 경우)
		    if (endX < 0 || startX >= screenSize.x)
		    {
			continue;
		    }

		    // 화면 내에 그려질 범위 계산
		    const int visibleStart = startX < 0 ? 0 : startX;
		    const int visibleEnd = endX >= screenSize.x ? screenSize.x - 1 : endX;


			// 문자열 설정.
			for (int x = visibleStart; x <= visibleEnd; ++x)
			{
				// 문자열 안의 문자 인덱스.
				const int sourceIndex = x - startX;

				if (sourceIndex >= length) break;

				// 프레임 (2차원 문자 배열) 인덱스.
				const int index
					= (command.position.y * screenSize.x) + x;

				// 그리기 우선순위 비교.
				if (frame->sortingOrderArray[index]
					> command.sortingOrder)
				{
					continue;
				}

				// 데이터 기록.
				frame->charInfoArray[index].Char.AsciiChar
					= command.text[sourceIndex];
				frame->charInfoArray[index].Attributes
					= (WORD)command.color;

				// 우선순위 업데이트.
				frame->sortingOrderArray[index]
					= command.sortingOrder;
			}
		}

		// 그리기.
		GetCurrentBuffer()->Draw(frame->charInfoArray);

		// 버퍼 교환.
		Present();

		// 렌더 큐 비우기.
		renderQueue.clear();
	}

	Renderer& Renderer::Get()
	{
		if (!instance)
		{
			MessageBoxA(
				nullptr,
				"Renderer::Get() - instance is null",
				"Error",
				MB_OK
			);

			__debugbreak();
		}

		return *instance;
	}

	void Renderer::Clear()
	{
		// 화면 지우기.
		// 1. 프레임(2차원 배열 데이터) 지우기.
		frame->Clear(screenSize);

		// 2. 콘솔 버퍼 지우기.
		GetCurrentBuffer()->Clear();
	}

	//void Renderer::Submit(
	//	const char* text,
	//	const Vector2& position,
	//	Color color,
	//	int sortingOrder)
	//{
	//	// 렌더 데이터 생성 후 큐에 추가.
	//	RenderCommand command = {};
	//	command.text = text;
	//	command.position = position;
	//	command.color = color;
	//	command.sortingOrder = sortingOrder;

	//	renderQueue.emplace_back(command);
	//}
	void Renderer::Submit(
	    const char* text,
	    const Vector2& position,
	    Color color,
	    int sortingOrder)
	{
	    if (!text) return;

	    // [핵심 로직] 문자열을 순회하며 줄바꿈(\n) 단위로 쪼개서 Command 생성
	    // 원본 문자열(text)은 메모리 어딘가에 살아있다고 가정하므로 포인터만 이동시킴.

	    const char* currentPtr = text;
	    int lineOffsetY = 0;

	    while (*currentPtr)
	    {
		// 현재 줄의 시작 지점
		const char* lineStart = currentPtr;
		int lineLength = 0;

		// 줄바꿈이나 문자열 끝을 만날 때까지 길이 측정
		while (*currentPtr && *currentPtr != '\n')
		{
		    currentPtr++;
		    lineLength++;
		}

		// 내용이 있는 줄만 등록
		if (lineLength > 0)
		{
		    RenderCommand command;
		    command.text = lineStart;       // 해당 줄의 시작 포인터
		    command.length = lineLength;    // 계산된 길이 (\n 제외)
		    command.position = Vector2(position.x, position.y + lineOffsetY);
		    command.color = color;
		    command.sortingOrder = sortingOrder;

		    renderQueue.emplace_back(command);
		}

		// 줄바꿈 문자를 만났다면 건너뛰고 Y 좌표 증가
		if (*currentPtr == '\n')
		{
		    currentPtr++;
		    lineOffsetY++;
		}
	    }
	}
	
	void Renderer::PresentImmediately()
	{
		Draw();
		GetCurrentBuffer()->Draw(frame->charInfoArray);
		Present();
	}

	void Renderer::Present()
	{
		// 버퍼 교환.
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

		// 인덱스 교체.
		currentBufferIndex = 1 - currentBufferIndex;
	}

	ScreenBuffer* Renderer::GetCurrentBuffer()
	{
		return screenBuffers[currentBufferIndex];
	}
}