#pragma once
#include <list>
#include <vector>
#include <SDL3/SDL.h>


/**
 * @struct WaterRippleParams
 * @brief ��������Ч������
 * @details ���嵥�����Ƶ��������Ժ�λ����Ϣ
 */
struct WaterRippleParams
{
	float amplitude = 0.0f;				///< ���������Ӱ�첨��ǿ�ȣ�
	float frequency = 0.0f;				///< ����Ƶ�ʣ����Ʋ����ٶȣ�
	float density = 0.0f;				///< �����ܶȣ�Ӱ�촫��˥����
	SDL_FPoint pos = { 0.0f, 0.0f };		///< ��������λ������
};

/**
 * @struct WaterClickRippleParams
 * @brief ��������Ĳ��Ʋ���
 * @details ������̬���Ƶ�ʱ�����Ժͻ����������
 */
struct WaterClickRippleParams
{
	WaterRippleParams rippleParams;		///< ���������������
	float startTime = 0.0f;				///< ������ʼʱ�䣨��λ���룩
	float lifeTime = 0.0f;				///< �����������ڣ���λ���룩
};

/**
 * @struct WaterWaveParams
 * @brief ֱ�ߴ������Ʋ���
 * @details �������ض����򴫲��Ĳ�������
 */
struct WaterWaveParams
{
	float amplitude = 0.0f;				///< ����ǿ��
	float frequency = 0.0f;				///< ����Ƶ��
	float density = 0.0f;				///< �����ܶ�ϵ����ֵԽ����Խ�ܼ���
	float angle = 0.0f;					///< ��������Ƕȣ������ƣ�
	float phi = 0.0f;					///< ��λƫ����
};

/**
 * @struct WaterWaveCalculatedParams
 * @brief Ԥ����Ĳ��Ʋ���
 * @details �洢ֱ�߲��Ƶķ�������Ԥ������
 */
struct WaterWaveCalculatedParams
{
	float A = 0.0f;						///< ��������X������cos(angle)��
	float B = 0.0f;						///< ��������Y������-sin(angle)��
	WaterWaveParams basicParams;			///< �������Ʋ���
};

/**
 * @struct WaterLightParams
 * @brief ����Ч������
 * @details ���Ʋ��Ƶ�͸���ȱ仯�͹���ģ��Ч��
 */
struct WaterLightParams
{
	float minDistance = 0.0f;			///< ��Сλ����ֵ�����ڴ�ֵ������͸���ȱ仯��
	float defaultAlpha = 1.0f;			///< Ĭ��͸����ֵ
	float minAlpha = 0.0f;				///< ��С͸����ֵ
	float decay = 10.0f;					///< ͸����˥��ϵ����ֵԽ��˥��Խ����
	float Angle = 0.0f;					///< ���սǶȣ������ƣ�
};

/**
 * @struct WaterEffectParams
 * @brief ˮ����Ч��ȫ�ֲ�������
 * @details �������в���Ч�������ò�������Ⱦ����
 */
struct WaterEffectParams
{
	int gridSize = 30;								///< ����ֱ��ʣ�ֵԽ����Խ��ϸ�����ܿ���Խ��
	std::list<WaterWaveCalculatedParams> waves;		///< ֱ�߲��Ʋ����б�
	std::list<WaterRippleParams> fixedRipples;		///< �̶�λ�ò����б�
	int maxClickRipple = 5;							///< ���ͬʱ���ڵĵ����������
	std::list<WaterClickRippleParams> clickRipples;	///< ��Ծ�ĵ�������б�
	WaterClickRippleParams defaultClickRipple;		///< Ĭ�ϵ�����Ʋ���ģ��
	WaterLightParams light;							///< ����Ч������
};

class WaterEffect
{
public:

	/**
	 * @brief ���캯��ʵ��
	 * @param window SDL���ڶ���
	 * @param renderer SDL��Ⱦ������
	 */
	WaterEffect(SDL_Window* window, SDL_Renderer* renderer);

	/**
	 * @brief ׼����Ч��Ⱦ����
	 * @details ���浱ǰ��ȾĿ�꣬����ˮ��������Ϊ��ȾĿ�겢��ջ���
	 */
	void setupEffectCanvas();

	/**
	 * @brief ��Ⱦˮ����Ч��
	 * @param time ��ǰʱ�䣨�룩
	 * @details 1. �ָ�ԭʼ��ȾĿ�� 2. ���¶���λ�� 3. ��Ⱦ���Ƽ�����
	 */
	void renderEffect(float time);

	/**
	 * @brief Ӧ��Ԥ���������
	 * @details ���ð�������ֱ�߲���+һ��̶����Ƶ�Ĭ��Ч��
	 */
	void applyPresetParams();


	/**
	 * @brief ��ʼ��ˮ��������
	 * @details ���ݸ�����С������ֱ��ʴ����������ݣ�
	 * 1. ������Ⱦ����
	 * 2. �������񶥵�λ��
	 * 3. ��������������
	 * @note ��������ߴ�仯����Ҫ���µ��ô˺���
	 */
	void initGrid(int width, int height);

	/**
	 * @brief ��������ˮ���Ʋ���ΪĬ��ֵ
	 * @details ��������Զ��岨�����ã�����ֱ�߲��ơ��̶����ƺ͵�����ƣ����ָ�Ϊ��ʼ״̬
	 */
	void clearParams();

	/**
	 * @brief ����ˮ��������ķֱ���
	 * @param gridSize ����ߴ磨�������0��
	 * @details �޸������С����Զ����³�ʼ�����񶥵�����
	 * @note ����ߴ�����Ӱ�����ܣ���С�ή�Ͳ���Ч������
	 */
	void setGridSize(int gridSize);

	/**
	 * @brief ���ֱ�ߴ����Ĳ���Ч��
	 * @param params ֱ�߲��Ʋ�������
	 * @details ���ݽǶȲ������㷨�������洢Ԥ��������
	 */
	void addWave(WaterWaveParams params);

	/**
	 * @brief ��ӹ̶�λ�õĲ���Դ
	 * @param params �̶����Ʋ�������
	 * @details �̶����ƻ�����������β���Ч��
	 */
	void addFixedRipple(WaterRippleParams params);

	/**
	 * @brief ����Ĭ�ϵĵ�����Ʋ���ģ��
	 * @param params �������Ĭ������
	 * @details �����ý����ں���ͨ�� addDefaultClickRipple ��ӵĲ���
	 */
	void setDefaultClickRippleParams(WaterClickRippleParams params);

	/**
	 * @brief ����Զ���������Ч��
	 * @param params �������������������
	 * @details ֱ��ʹ�ô����������������ƣ�����Ĭ�ϲ���Ӱ��
	 */
	void addClickRipple(WaterClickRippleParams params);

	/**
	 * @brief ʹ��Ĭ�ϲ�����ӵ������
	 * @param x ���λ�õ�X����
	 * @param y ���λ�õ�Y����
	 * @param startTime ������ʼʱ�䣨�룩
	 * @details ����Ԥ���Ĭ�ϲ�����ָ��λ�����ɵ������
	 */
	void addDefaultClickRipple(float x, float y, float startTime);

	/**
	 * @brief ������ɫЧ������
	 * @param params ��ɫ���ò���
	 * @details ���Ʋ��Ƶ�͸���ȱ仯Ч��������ģ�⣩
	 */
	void setLightParams(WaterLightParams params);

	/**
	 * @brief �����������������
	 * @param count ����ͬʱ���ڵ������������
	 * @details ������������ʱ���µĵ�����ƽ�������
	 */
	void setMaxClickRipple(int count);

private:
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;

	WaterEffectParams _params;

	SDL_Texture* _originalRenderTarget = nullptr;
	SDL_Texture* _waterEffectCanvas = nullptr;
	std::vector<SDL_Vertex> _vertices;
	std::vector<SDL_Vertex> _verticesOrigin;
	std::vector<SDL_Vertex> _verticesLast;
	std::vector<int> _indices;

	/**
	 * @brief ���²���Ч����ÿ֡���ã�
	 * @param time ��ǰʱ�䣨�룩
	 * @details �������в��Ƶĵ���Ч�������¶���λ�ã�
	 * 1. ����ֱ�߲���ƫ��
	 * 2. ����̶�����ƫ��
	 * 3. ���µ�����ƣ����������ڹ���
	 * 4. Ӧ�ñ߽�Լ��
	 * 5. ���㶥��͸���ȱ仯
	 */
	void _update(float time);

};

